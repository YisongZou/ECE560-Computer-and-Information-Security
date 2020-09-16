#include <bits/stdc++.h>
#include <crypto++/aes.h>
#include <crypto++/cryptlib.h>
#include <crypto++/filters.h>
#include <crypto++/hex.h>
#include <crypto++/modes.h>
#include <crypto++/osrng.h>
#include <crypto++/sha.h>
#include <iomanip>
#include <iostream>
using namespace std;
using namespace CryptoPP;
AutoSeededRandomPool prng;

string readFileIntoString(string filename) {
  ifstream ifile(filename);
  ostringstream buf;
  char ch;
  while (buf && ifile.get(ch))
    buf.put(ch);
  return buf.str();
}

void write_byte_to_file(string file_string, string str) {
  FILE *pFile;
  pFile = fopen(file_string.c_str(), "a+b");
  fwrite(str.c_str(), sizeof(char), str.size(), pFile);
  fclose(pFile);
}

void establish_new_file(const std::string &file_string) {
  std::ofstream OsWrite(file_string);
  string str;
  OsWrite << str;
  OsWrite.close();
}

void write_string_to_file(const std::string &file_string, string str) {
  std::ofstream OsWrite(file_string);
  OsWrite << str;
  OsWrite.close();
}

int main(int argc, char *argv[]) {
  if (argc != 4) {
    string output = R"(Syntax:
  To encrypt:  ./duke-crypter -e <input_file> <output_file>
  To decrypt:  ./duke-crypter -d <input_file> <output_file>
)";
    cout << output;
    return EXIT_FAILURE;
  }
  string funcType = argv[1];
  string inputFile = argv[2];
  string outputFile = argv[3];
  //////////////////////////////////////////////////////////////////////////
  // Encrypt
  if (funcType == "-e") {
    // Read the user input key
    string plainKey;
    cout << "Please input a secret key:" << endl;
    cin >> plainKey;
    // Hash the plainKey and convert it to byte*
    SecByteBlock key(AES::DEFAULT_KEYLENGTH);
    std::string keyStr;
    SHA256 hash;
    hash.Update((const byte *)plainKey.data(), plainKey.size());
    keyStr.resize(hash.DigestSize() / 2);
    hash.TruncatedFinal((byte *)&keyStr[0], keyStr.size());
    for (int i = 0; i < 16; ++i) {
      key[i] = keyStr[i];
    }
    //-------------------------------------------------------------
    // Write to the file
    // Pretty print cipher text
    string hexKey;
    StringSource s1(keyStr, true,
                    new HexEncoder(new StringSink(hexKey)) // HexEncoder
    );                                                     // StringSource
    cout << "key: " << hexKey << endl;
    //-------------------------------------------------------------
    // Generate a random IV
    byte iv[AES::BLOCKSIZE];
    prng.GenerateBlock(iv, sizeof(iv));
    string ivStr(16, '.');
    for (int i = 0; i < 16; ++i) {
      ivStr[i] = iv[i];
    }
    //-------------------------------------------------------------
    // Write to the file
    // Pretty print cipher text
    string hexIv;
    StringSource sw(ivStr, true,
                    new HexEncoder(new StringSink(hexIv)) // HexEncoder
    );                                                    // StringSource
    cout << "iv: " << hexIv << endl;
    //-------------------------------------------------------------
    // get the plain text
    string plain = readFileIntoString(inputFile);
    string cipher;
    try {
      // Encrypt
      CBC_Mode<AES>::Encryption e;
      e.SetKeyWithIV(key, keyStr.size(), iv);
      // The StreamTransformationFilter adds padding
      //  as required. ECB and CBC Mode must be padded
      //  to the block size of the cipher.
      StringSource s0(plain, true,
                      new StreamTransformationFilter(
                          e,
                          new StringSink(cipher)) // StreamTransformationFilter
      );                                          // StringSource
      byte cypherBytes[34864];
      for (int i = 0; i < 34864; ++i) {
        cypherBytes[i] = cipher[i];
      }
      // Hash the plaintext for integrity
      string plainHash;
      hash.Update((const byte *)plain.data(), plain.size());
      plainHash.resize(hash.DigestSize() / 2);
      hash.TruncatedFinal((byte *)&plainHash[0], plainHash.size());
      byte hashBytes[16];
      for (int i = 0; i < 16; ++i) {
        hashBytes[i] = plainHash[i];
      }
      // Write to the file
      establish_new_file(outputFile);
      write_byte_to_file(outputFile, ivStr);
      write_byte_to_file(outputFile, cipher);
      write_byte_to_file(outputFile, plainHash);
      cout << "ivStr size " << ivStr.size() << endl;
      cout << "cipher size " << cipher.size() << endl;
      cout << "plainHash size " << plainHash.size() << endl;
    } catch (const CryptoPP::Exception &e) {
      cerr << e.what() << endl;
      exit(1);
    }
  }
  //////////////////////////////////////////////////////////////////////////
  // // Decrypt
  if (funcType == "-d") {
    // Read the user input key
    string plainKey;
    cout << "Please input a secret key:" << endl;
    cin >> plainKey;
    // Hash the plainKey and convert it to byte*
    SecByteBlock key(AES::DEFAULT_KEYLENGTH);
    std::string keyStr;
    SHA256 hash;
    hash.Update((const byte *)plainKey.data(), plainKey.size());
    keyStr.resize(hash.DigestSize() / 2);
    hash.TruncatedFinal((byte *)&keyStr[0], keyStr.size());
    for (int i = 0; i < 16; ++i) {
      key[i] = keyStr[i];
    }
    //-------------------------------------------------------------
    // Write to the file
    // Pretty print cipher text
    string encoded1;
    StringSource s1(keyStr, true,
                    new HexEncoder(new StringSink(encoded1)) // HexEncoder
    );                                                       // StringSource
    cout << "key: " << encoded1 << endl;
    //-------------------------------------------------------------
    // Get the whole message bytes
    string message = readFileIntoString(inputFile);
    // Get the IV
    byte iv[16];
    for (int i = 0; i < 16; ++i) {
      iv[i] = message[i];
    }
    string ivStr(16, '.');
    for (int i = 0; i < 16; ++i) {
      ivStr[i] = iv[i];
    }
    // Write to the file
    //-------------------------------------------------------------
    // Pretty print cipher text
    string encoded;
    StringSource sw(ivStr, true,
                    new HexEncoder(new StringSink(encoded)) // HexEncoder
    );                                                      // StringSource
    cout << "iv: " << encoded << endl;
    //-------------------------------------------------------------
    // get the Crypted plain text
    string cipher(34864, '.');
    for (int i = 16; i < 34880; ++i) {
      cipher[i - 16] = message[i];
    }
    // Get the hash
    string h(16, '.');
    for (int i = 34880; i < 34896; ++i) {
      h[i - 34880] = message[i];
    }
    string recovered;
    try {
      // Decrypt
      CBC_Mode<AES>::Decryption d;
      d.SetKeyWithIV(key, keyStr.size(), iv);

      // The StreamTransformationFilter removes
      //  padding as required.
      StringSource ss(
          cipher, true,
          new StreamTransformationFilter(
              d,
              new StringSink(recovered)) // StreamTransformationFilter
      );                                 // StringSource
      // Hash the plaintext for integrity
      string plainHash;
      hash.Update((const byte *)recovered.data(), recovered.size());
      plainHash.resize(hash.DigestSize() / 2);
      hash.TruncatedFinal((byte *)&plainHash[0], plainHash.size());
      byte hashBytes[16];
      for (int i = 0; i < 16; ++i) {
        hashBytes[i] = plainHash[i];
      }
      // detect the tampering
      if (plainHash != h) {
        exit(1);
      }
      write_string_to_file(outputFile, recovered);
    } catch (const CryptoPP::Exception &e) {
      cerr << e.what() << endl;
      exit(1);
    }
  }
  return EXIT_SUCCESS;
}