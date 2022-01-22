#define _CRT_SECURE_NO_WARNINGS

#include <locale.h>
#include "../Api/cmd.h"

/// <summary>
///		Entry point
/// </summary>
/// 
/// <returns>int</returns>
int main()
{
	setlocale(LC_ALL, "Russian");
	CommandLine::Create();

	return 0;
}