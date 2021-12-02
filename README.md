# runp :: Run a binary payload in a wrapper, for easier analysis in debuggers.

Typically, binary payloads are non-executable in their raw state, as they lack PE and DOS headers. This utility provides a "wrapper" of sorts, allowing for the execution and dynamic analysis of binary payloads, such as those found in PowerShell "fileless" malware attacks.

## Usage

To execute a binary payload, simply pass its path as a command-line argument to `runp.exe`:

`runp.exe c:\demo\payload.bin`

## Compilation

`cl.exe /Od /GS- /nologo /EHsc /Zi /Fe: C:\path\to\runp.exe C:\path\to\runp.cpp`

## Warnings

* The payload will be executed without any warning or pause.
    * It is best to launch `runp.exe` from inside a debugger, so it loads in a paused state.
* The `runp.exe` binary will be flagged by most antivirus applications as malware.
    * This is because it employs the same tactics malware uses to execute the binary payload.
    * The code in `runp.exe` is benign; without a payload, it will do nothing.

Basically, treat `runp.exe` like a gun. If it's not loaded, it's safe. If it's loaded (with a payload), it is only as dangerous as the payload it executes.

## Crafting Payloads

The `runp.exe` utility accepts binary payloads as seen in fileless malware attacks. These payloads are essentially pure shellcode, with no PE or DOS headers. Experienced shellcoders can create custom payloads from scratch, but if you want a quick way to generate basic payloads, the `msfvenom` tool can be used for this purpose. For an example of a Proof of Concept payload that "pops calc," see the `msfvenom.txt` file.
