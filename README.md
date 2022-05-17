# pkgnote

A utility to read an ELF file and print its packaging metadata. It looks for the note section header of type `0xCAFE1A7E` (`.note.packaging`) and extracts the owner and value, following the specification.

**Section header**

```
SECTION: `.note.package`
note type: `0xcafe1a7e`
Owner: `FDO` (FreeDesktop.org)
Value: a single JSON object encoded as a zero-terminated UTF-8 string
```

**Value example**

```json
{
    "type": "rpm",
    "name": "coreutils",
    "version": "9.0-5.fc36",
    "architecture": "x86_64",
    "osCpe": "cpe:/o:fedoraproject:fedora:36"
}
```

Run `pkgnote FILE` to print the JSON object or `pkgnote -o FILE` to print the owner.

See [Package Metadata for Core Files](https://systemd.io/COREDUMP_PACKAGE_METADATA) for the overview and details.

## Building from source

**Requirements**

+ make
+ gcc
+ libelf

```
git clone https://github.com/xfgusta/pkgnote
cd pkgnote
make
```

## TODO

+ Use json-c to parse the JSON object
    + Add a -p option to pretty print
    + Add a -k option to print a specific key