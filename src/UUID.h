#include <iostream>
#include <sodium.h>
#include <string>

class UUID {
private:
	static std::string generateUUID() {
		if (sodium_init() < 0) {
			throw  "libsodium initialization failed.";
		}
		constexpr size_t UUID_SIZE = 16;
		unsigned char uuid[UUID_SIZE];
		randombytes(uuid, UUID_SIZE);
		std::string uuidString;
		for (size_t i = 0; i < UUID_SIZE; ++i) {
			char buffer[3];
			snprintf(buffer, sizeof(buffer), "%02x", uuid[i]);
			uuidString += buffer;
		}
		return uuidString;
	}


public:
	static std::string GetUUID() {
		return generateUUID();
	}

};
