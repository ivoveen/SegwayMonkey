#include "Zone.h"
#include "Chunk.h"
#include "xorshift32.h"

Zone::Zone(ObjectShader* shaderManager, BulletManager* bulletManager, int zoneID, int difficulty) {
	mpShaderManager = shaderManager;
	mpBulletManager = bulletManager;
	mZoneID = zoneID;
	seed = static_cast<uint>(time(NULL)) + zoneID;
	GenerateZone(difficulty);
}

Zone::~Zone() {
	for (int i = 0; i < mZoneChunks.size(); i++) {
		delete mZoneChunks[i];
	}
}
bool Zone::GenerateZone(int difficulty) {
	GenerateName();

	//generate zoneLength
	seed = RandomUInt(seed);
	int zoneLength = minimumAmountChunks + (seed % (maximumAmountChunks + 1 - minimumAmountChunks));
	float factor = (seed % 101);
	factor /= 100;
	mColor = mix(vec3(126, 200, 80), vec3(19, 109, 21), factor); //choose a shade of green

	mColor /= 255;
	for (int i = 0; i < zoneLength; i++) {
		mZoneChunks.push_back(new Chunk(mpShaderManager, mpBulletManager, mTypeOfZone, mZoneID, i, zoneLength - 1, seed, mColor, difficulty));
	}

	return true;
}

void Zone::GenerateName() {
	int minimumAmountOfPairs = 2;
	int maximumAmountOfPairs = 4;

	char* vowels[5] = { "a", "e", "i", "o", "u" }; // banned "y" and "q" for making my names uglier
	char* consonants[19] = { "b", "c", "d", "f", "g", "h", "j", "k", "l", "m", "n", "p", "r", "s", "t", "v", "w", "x", "z" };
	char* vowel;
	char* consonant;
	std::string word = "";
	seed = RandomUInt(seed);
	bool startVowel = seed % 2;
	int number = minimumAmountOfPairs + (seed % (maximumAmountOfPairs + 1 - minimumAmountOfPairs));
	for (int i = 0; i < number; i++) {
		seed = RandomUInt(seed);
		vowel = vowels[(seed % (5))];
		consonant = consonants[(seed % (19))];
		if (startVowel) {
			word = word + vowel + consonant;
		}
		else {
			word = word + consonant + vowel;
		}
	}
	mZoneName = word;
	printf("Seed: %i \n Very pronounceable name: ", seed);
	printf(word.c_str());
	printf("\n");
}

void Zone::Update(float deltaTime) {
	
	for (int i = 0; i < mZoneChunks.size(); i++) {
		mZoneChunks[i]->Update(deltaTime);
	}
}
void Zone::Draw() {
	for (int i = 0; i < mZoneChunks.size(); i++) {
		mZoneChunks[i]->Draw();
	}
}

//XOR shift-32
