#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <format>


std::wstring ConvertString(const std::string& str);

std::string ConvertString(const std::wstring& str);

class Log {
public:
	static void View(const std::string& message);
	static void ViewFile(const std::string& message);
	static void Initialize();
private:
	
private:
	static std::ofstream logStream_;
};

// 将来的に入れるときは何でもよくて、関数内で勝手に処理するようにしたい

