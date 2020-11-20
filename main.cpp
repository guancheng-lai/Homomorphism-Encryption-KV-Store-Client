#include <iostream>
#include <sstream>
#include <fstream>
#include "rpc/client.h"
#include "seal/seal.h"
#include "hash.hpp"

using namespace std;
using namespace seal;

double hexToDecimal(const std::string& input) {
  std::stringstream ss;
  ss << std::hex << input;
  double res;
  ss >> res;
  return res;
}

void init(std::shared_ptr<seal::Evaluator>& evaluator,
          std::shared_ptr<seal::Decryptor>& decryptor,
          std::shared_ptr<seal::Encryptor>& encryptor,
          std::shared_ptr<seal::SEALContext>& context) {
  seal::EncryptionParameters parms(seal::scheme_type::bfv);
  size_t poly_modulus_degree = 4096;
  parms.set_poly_modulus_degree(poly_modulus_degree);
  parms.set_coeff_modulus(seal::CoeffModulus::BFVDefault(poly_modulus_degree));
  parms.set_plain_modulus(1024);
  context = std::make_shared<SEALContext>(parms);

  fstream fs_pk("public.bin", fstream::in | fstream::binary);
  fstream fs_sk("secret.bin", fstream::in | fstream::binary);
  if (fs_pk.fail() || fs_sk.fail()) {
    // Generate key and write into files
    seal::KeyGenerator keygen(*context);
    seal::PublicKey public_key;
    keygen.create_public_key(public_key);
    seal::SecretKey secret_key = keygen.secret_key();
    fs_pk.open("public.bin", fstream::out | fstream::binary);
    public_key.save(fs_pk);
    fs_pk.flush();
    fs_sk.open("secret.bin", fstream::out | fstream::binary);
    secret_key.save(fs_sk);
    fs_sk.flush();
    evaluator = std::make_shared<seal::Evaluator>(*context);
    encryptor = std::make_shared<seal::Encryptor>(*context, public_key);
    decryptor = std::make_shared<seal::Decryptor>(*context, secret_key);
  } else {
    // Use the existed key
    seal::PublicKey public_key;
    public_key.load(*context, fs_pk);
    seal::SecretKey secret_key;
    secret_key.load(*context, fs_sk);
    evaluator = std::make_shared<seal::Evaluator>(*context);
    encryptor = std::make_shared<seal::Encryptor>(*context, public_key);
    decryptor = std::make_shared<seal::Decryptor>(*context, secret_key);
  }
  fs_pk.close();
  fs_sk.close();
}

std::vector<unsigned char> to_vector(std::stringstream& ss)
{
  // discover size of data in stream
  ss.seekg(0, std::ios::beg);
  auto bof = ss.tellg();
  ss.seekg(0, std::ios::end);
  auto stream_size = std::size_t(ss.tellg() - bof);
  ss.seekg(0, std::ios::beg);

  // make your vector long enough
  std::vector<unsigned char> v(stream_size);

  // read directly in
  ss.read((char*)v.data(), std::streamsize(v.size()));

  return v;
}

std::stringstream to_stream(std::vector<unsigned char> const& v)
{
  std::stringstream ss;
  ss.write((char const*)v.data(), std::streamsize(v.size()));
  return ss;
}

void Set(rpc::client& client, stringstream& ss, shared_ptr<seal::Encryptor> encryptor, const string& store) {
  int value;
  string product;
  ss >> product >> value;
  cout << "Calling Rpc Set(" << product << ", " <<  value << ")" << endl;

  seal::Ciphertext ciphertext;
  encryptor->encrypt(seal::Plaintext(to_string(value)), ciphertext);

  string data_represent = ciphertext.to_string();
  if (data_represent.size() > 80) {
    data_represent.resize(80);
  }
  cout << "Encoding and encrypting: " << data_represent << "......" << endl;
  cout << "Sending encrypted data to server...";
  stringstream oss;
  ciphertext.save(oss);
  client.call("Set", store, product, to_vector(oss));
  cout << "Succeed!" << endl;
}

void Get(rpc::client& client, stringstream& ss, shared_ptr<seal::Decryptor> decryptor, shared_ptr<seal::SEALContext> context, const string& store) {
  string product;
  ss >> product;
  cout << "Calling Rpc Get(" << product << ")" << endl;
  auto [stream, cnt] = client.call("Get", store, product).as<std::pair<vector<unsigned char>, int>>();
  double res = 0.0;
  if (cnt > 0) {
    Ciphertext ciphertext;
    stringstream recv_stream = to_stream(stream);
    ciphertext.load(*context, recv_stream);
    recv_stream.flush();

    string dataRepresent = ciphertext.to_string();
    dataRepresent.resize(80);
    cout << "Received encrypted data! Decoding and decrypting: " << dataRepresent + "......" << endl;

    seal::Plaintext decrypted_result;
    decryptor->decrypt(ciphertext, decrypted_result);
    res = hexToDecimal(decrypted_result.to_string()) / cnt;
  }
  std::cout << "Total " << cnt << " record(s), the avg is: " << res << std::endl;
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Invalid arguement" << endl;
    return 0;
  }

  // Login to the server
  string store(argv[1]), password(argv[2]);
  rpc::client client("3.12.237.48", 8081);
  if (!client.call("Login", store, Hash(password.data(), password.size(), 0)).as<bool>()) {
    cout << "Failed to login" << endl;
    return 0;
  }
  cout << "Login as: " << store << endl;

  // Initialization
  std::shared_ptr<seal::Evaluator> evaluator;
  std::shared_ptr<seal::Decryptor> decryptor;
  std::shared_ptr<seal::Encryptor> encryptor;
  std::shared_ptr<seal::SEALContext> context;
  init(evaluator, decryptor, encryptor, context);

  // Loop for operations
  string input;
  while (getline(cin, input)) {
    stringstream ss;
    ss << input;
    string task;
    ss >> task;
    if (task == "exit") {
      break;
    } else if (task == "set") {
      Set(client, ss, encryptor, store);
    } else if (task == "get") {
      Get(client, ss, decryptor, context, store);
    } else {
      cout << "Invalid input" << endl;
    }
    cout << "----------------------------------------------------------------" << endl;
  }

  return 0;
}
