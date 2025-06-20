#include "tclap/CmdLine.h"
#include <iostream>
#include <stdexcept>

#ifndef PROGRAM_VERSION
#define PROGRAM_VERSION "0.1"
#endif

#define INRANGE(x, a, b) (((a) <= (x)) && ((x) <= (b)))

using namespace TCLAP;

struct HexerArgs {
    bool read;
    bool format;
    bool strict;
};

struct IO {
    std::istream &in;
    std::ostream &out;
};

void write_hex(IO io, char byte) {
    for (int i = 1; i >= 0; i--) {
        char nibble = (byte >> (i * 4)) & 0xF;
        io.out << std::hex << (int)nibble;
    }
}

char decode_hex_digit(char d) {
    if (INRANGE(d, '0', '9')) {
        return d - '0';
    }

    if (INRANGE(d, 'A', 'F')) {
        d += 'a' - 'A';
    }

    if (INRANGE(d, 'a', 'f')) {
        return 10 + d - 'a';
    }

    throw std::runtime_error("Fatal error. Could not decode hex digit: " +
                             std::to_string(d));
}

char decode_hex(char d1, char d2) {
    d1 = decode_hex_digit(d1);
    d2 = decode_hex_digit(d2);
    return (d1 << 4) + d2;
}

IO getIO(HexerArgs args) {
    return {std::cin, std::cout};
}

void bin2hex(HexerArgs args) {
    IO io = getIO(args);
    int position = 0;

    while (true) {
        char byte = io.in.get();
        if (io.in.eof())
            break;

        if (args.format && position != 0) {
            if ((position & 0b111) == 0) {
                io.out << std::endl;
            } else if ((position & 0b11) == 0) {
                io.out << "  ";
            } else {
                io.out << " ";
            }
        }

        write_hex(io, byte);
        position++;
    }

    if (args.format) {
        io.out << std::endl;
    }
}

bool is_hex_digit(char byte) {
    return INRANGE(byte, '0', '9') || INRANGE(byte, 'a', 'f') ||
           INRANGE(byte, 'A', 'F');
}

bool is_whitespace(char byte) {
    switch (byte) {
    case ' ':
    case '\t':
    case '\n':
    case '\r':
        return true;
    default:
        return false;
    }
}

char read_next_hex_digit(IO io, HexerArgs args) {
    while (true) {
        char byte = io.in.get();
        if (io.in.eof())
            break;

        if (is_hex_digit(byte))
            return byte;

        if (args.strict && !is_whitespace(byte)) {
            throw std::runtime_error("The character " + std::string(1, byte) +
                                     " (" + std::to_string(byte) +
                                     ") is not a valid hex digit");
        }
    }

    throw std::runtime_error(
        "EOF before even-numbered hex digit (there has to be an "
        "even number of valid hex digits)");
}

bool try_read_next_hex_digit(IO io, std::ostream *error, HexerArgs args,
                             char &digit) {
    try {
        digit = read_next_hex_digit(io, args);
        return true;
    } catch (std::runtime_error &e) {
        if (error != nullptr)
            *error << "Could not decode hex stream properly. Reason: "
                   << e.what() << std::endl;
        return false;
    }
}

void hex2bin(HexerArgs args) {
    IO io = getIO(args);

    while (true) {
        char d1, d2;
        bool success;
        success = try_read_next_hex_digit(io, &std::clog, args, d1);
        if (!success)
            return;
        success = try_read_next_hex_digit(io, &std::clog, args, d2);
        if (!success)
            return;

        io.out << decode_hex(d1, d2);
    }
}

void process_args(HexerArgs args) {
    if (args.read) {
        hex2bin(args);
    } else {
        bin2hex(args);
    }
}

int main(int argc, char **argv) {
    HexerArgs args;
    try {
        CmdLine cmd(
            "The HEXer command. Takes something input and write stuff to "
            "output. Default is reading binary data and outputting the hex "
            "encoding of that.",
            ' ', PROGRAM_VERSION);
        SwitchArg format(
            "f", "format",
            "Do a little bit of formatting in the write-hex-mode to "
            "make that output more readable",
            cmd);
        SwitchArg read(
            "r", "read",
            "Read hexadecimal from the input stream and write the "
            "binary data to the output stream. Whitespace is ignored",
            cmd);
        SwitchArg strict(
            "s", "strict",
            "Only has an effect for -r. Instead of ignoring non-hex "
            "characters, the program terminates with an error. It "
            "still ignores whitespace ( ,\\t,\\n,\\r)",
            cmd);

        cmd.parse(argc, argv);

        args.read = read.getValue();
        args.format = format.getValue();
        args.strict = strict.getValue();
    } catch (ArgException &e) {
        std::clog << "Some error: " << e.what() << std::endl;
        return -1;
    }

    try {
        process_args(args);
    } catch (std::runtime_error &e) {
        std::clog << "Runtime error occurred: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
