#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <algorithm>
#include <string>
#include <boost/property_tree/ini_parser.hpp>

#include "robin_hood.h"

namespace utils
{

	class Dictionary
	{
	public:

		const static uint8_t CYRILLIC_A = 224; // The first letter of the cyrillic alphabet
		const static uint8_t BOX_CHAR = 209; // Used to place explanations in it
		const static uint8_t SPECIAL_BOX_CHAR = 208; // Used to place images over it

		const static uint8_t ANY_CHAR = 0; // Used in patterns to indicate that any character can be placed there
		const static uint8_t LONGEST_WORD = 50;
	
	public:

		class Pattern
		{
		public:
			Pattern(size_t size, std::vector<uint16_t>::iterator nextWord, std::function<const std::string& (uint16_t)> callback) :
				size(size),
				_get(callback),
				_nextWord(nextWord)
			{}
			Pattern(const Pattern& other) = default;
			Pattern(Pattern&& other) = default;

		public:
			size_t size; // The number of possible words
			const std::string& operator()() { return _get(*_nextWord++); } // Returns the next possible word

		private:
			std::function<const std::string& (uint16_t)> _get; // callback to dictionary which returns word from index.
			std::vector<uint16_t>::iterator _nextWord; // iterator given at construction pointing from vector of possible words.
		};

	public:

		static int levenstein(std::string a, std::string b); // Returns the distance between word 'a' and word 'b'

	public:

		inline const std::string& getDirty(const std::string& clean) { return _dirtyDict[clean]; }
		inline const std::string& getExplanation(const std::string& clean) { return _explanationDict[clean]; }
		Pattern findPossible(const std::string& pattern);

	public:

		Dictionary();
		~Dictionary();

	private:

		static std::string cleanString(const std::string& dirtyString);
		static std::string toupper(std::string word);
		static char toupper(char c);
		static bool isalpha(char c);
		static std::string dosToWinCode(std::string word);
		
		static uint64_t getKey(uint32_t mask, const std::string& word);
		static bool isPossible(const std::string& pattern, const std::string& contender);

		void addToFastSearch(const std::string& newWord, uint16_t index);
		inline const std::string& getFromIndex(uint16_t index) const { return _allWords[index]; }

		void loadDictionary();
		void reset();

	private:

		robin_hood::unordered_map<std::string, std::string> _explanationDict; // Given a clean word it returns an explination.
		robin_hood::unordered_map<std::string, std::string> _dirtyDict; // Given a clean word it returns the original untouched word in the dictionary.
		std::vector<std::string> _allWords; // All words loaded from the dict
		std::list<std::vector<uint16_t>> _cache; // Contains vector of shorts which point to words which match a certain pattern.
												 // Uses list because we need persistant iterators.
		robin_hood::unordered_map<std::string, std::list<std::vector<uint16_t>>::iterator> _cacheSearchMap; // Maps from pattern to cache
		robin_hood::unordered_map<uint64_t, std::vector<uint16_t>> _fastSearch[LONGEST_WORD]; // Maps from first six (or less if there aren't enough) letters to word indices. Divided by length.

		boost::property_tree::ptree _iniPropertyTree;

		std::string _dictionaryFilePath;

	};

}