#!/usr/bin/env python3
import argparse
import base64
import gzip
import hashlib
import json
import os
import pyhy
import shutil
import struct
import sys

# workflow:
# 1. prepare the binary (compress, sign)
# 2. upload
# 3. create the manifest and its signature
# 4. upload

def get_secret_key(args):
    sk_b64 = os.environ[args.secret_key_envvar_name]
    return base64.b64decode(sk_b64)


def generate_signing_keypair(args):
    kp = pyhy.hydro_sign_keygen()
    with open(args.pubkey_file, 'wb') as f:
        f.write(bytes(kp.pk))

    print(f'Public key was written to "{args.pubkey_file}".')
    sk_b64 = base64.b64encode(bytes(kp.sk)).decode('utf-8')
    print(f'Secret key to put to an environment variable of your CI: {sk_b64}')


def compress_file(filename_in, filename_out):
    with open(filename_in, 'rb') as fi:
        with gzip.open(filename_out, 'wb', compresslevel=9) as fo:
            shutil.copyfileobj(fi, fo)


def append_signature(filename, secret_key):
    signing_context = 'CmFwSkSg' # 8 character abbreviation of "CheckMeet Firmware Sketch Signature"
    signer = pyhy.hydro_sign(signing_context)
    with open(filename, 'rb') as f:
        for chunk in iter(lambda: f.read(4096), b''):
            signer.update(chunk)
    
    signature = signer.final_create(secret_key)

    with open(filename, 'ab') as f:
        f.write(signature)
        f.write(struct.pack('<L', len(signature)))


def prepare_binary(args):
    compress_file(args.unsigned_binary, args.output)
    append_signature(args.output, get_secret_key(args))
    print(f'The compressed and signed binary was written to "{args.output}".')


def md5_file(filename):
    hasher = hashlib.md5()
    with open(filename, 'rb') as f:
        for chunk in iter(lambda: f.read(4096), b''):
            hasher.update(chunk)

    return hasher.hexdigest()


def hydro_hash_file(filename):
    with open(filename, 'rb') as f:
        return pyhy.hydro_hash_hash(pyhy.hydro_hash_BYTES, f.read(), "CmFwSkSg").hex()


def create_manifest(args):
    manifest_object = {
        'tag': args.tag,
        'url': args.url_of_signed_binary,
        'sketch-hydro-hash': hydro_hash_file(args.unsigned_binary),
        'sketch-md5': md5_file(args.unsigned_binary),
        'commit': args.commit,
    }
    manifest_str = json.dumps(manifest_object, sort_keys=True, indent=4) + '\n'
    manifest_json = manifest_str.encode('utf-8')
    with open(args.output, 'wb') as f:
        f.write(manifest_json)

    print(f'The manifest was written to "{args.output}".')

    signing_context = 'CmFwMfSg' # 8 character abbreviation of "CheckMeet Firmware Manifest Signature"
    signature = pyhy.hydro_sign_create(manifest_json, signing_context, get_secret_key(args))
    filename = args.output + '.sig'
    with open(filename, 'wb') as f:
        f.write(signature)

    print(f'And its signature was written to "{filename}".')


def main():
    parser = argparse.ArgumentParser(description='A tool to prepare and sign a new firmware update')
    parser.add_argument('--version', action='version', version=', '.join([pyhy.hydro_version(), pyhy.pyhy_version()]))
    subparsers = parser.add_subparsers()

    parser_keygen = subparsers.add_parser('generate-signing-keypair')
    parser_keygen.add_argument('--pubkey-file', type=str, help='the name of the file to output public key to', required=True)
    parser_keygen.set_defaults(func=generate_signing_keypair)

    parser_prepare_binary = subparsers.add_parser('prepare-binary')
    parser_prepare_binary.add_argument('--unsigned-binary', type=str, help='the compiled binary file to be signed', required=True)
    parser_prepare_binary.add_argument('--secret-key-envvar-name', type=str, help='the name of the envvar that stores the secret key in base64', required=True)
    parser_prepare_binary.add_argument('--output', type=str, help='the name of the output file', required=True)
    parser_prepare_binary.set_defaults(func=prepare_binary)

    parser_create_manifest = subparsers.add_parser('create-manifest')
    parser_create_manifest.add_argument('--tag', type=str, help='tag name of the current version', required=True)
    parser_create_manifest.add_argument('--commit', type=str, help='git commit hash of the current version', required=True)
    parser_create_manifest.add_argument('--unsigned-binary', type=str, help='the not yet signed binary (for sketch chechsum calculation)', required=True)
    parser_create_manifest.add_argument('--url-of-signed-binary', type=str, help='the URL of the signed binary is publicly available at', required=True)
    parser_create_manifest.add_argument('--secret-key-envvar-name', type=str, help='the name of the envvar that stores the secret key in base64', required=True)
    parser_create_manifest.add_argument('--output', type=str, help='the name of the output file (signature\'s will be <OUTPUT>.sig)', required=True)
    parser_create_manifest.set_defaults(func=create_manifest)

    args = parser.parse_args()
    if 'func' not in args:
        parser.print_help()
        sys.exit(1)

    args.func(args)

if __name__ == '__main__':
    main()
