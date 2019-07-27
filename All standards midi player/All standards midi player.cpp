// All standards midi player.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include <iostream>
#include <string>
#include <vector>
#include "bass.h"
#include "bassmidi.h"
using namespace std;

HSOUNDFONT soundfonts[3];
vector<BASS_MIDI_FONTEX> font_info;
HSTREAM stream;
string exe_path, resources_path;
float filterRC = 1.0 / (20.0 * 6.283185307179586476925286766559);
vector<float> filter_prev_filtered, filter_prev;

void CALLBACK filter20Hz_f(
	HDSP handle,
	DWORD channel,
	void* buffer,
	DWORD length,
	void* user)
{
	BASS_CHANNELINFO info;
	BASS_ChannelGetInfo(channel, &info);
	filter_prev.resize(info.chans, 0.0f);
	filter_prev_filtered.resize(info.chans, 0.0f);
	float dt = float(1.0 / info.freq);
	float alpha = filterRC / (filterRC + dt);
	float* ptr = (float*)buffer;
	float* end = (float*)((byte*)(buffer) + length);
	while (ptr != end)
	{
		for (DWORD i = 0; i < info.chans; i++)
		{
			float filtered = alpha * (filter_prev_filtered[i] + (*ptr) - filter_prev[i]);
			filter_prev[i] = *ptr;
			filter_prev_filtered[i] = filtered;
			*ptr = filtered;
			ptr++;
		}
	}
}

int main(int argc, char** argv)
{
	exe_path = argv[0];
	resources_path = exe_path.substr(0, exe_path.find_last_of("/\\") + 1);

	cout << argv[1] << endl;

	BASS_Init(-1, 48000, NULL, NULL, NULL);
	cout << BASS_ErrorGetCode() << endl;

	soundfonts[0] = BASS_MIDI_FontInit(string(resources_path + "LSB bank.sf2").c_str(), NULL);
	cout << BASS_ErrorGetCode() << endl;
	BASS_MIDI_FontSetVolume(soundfonts[0], 1.25f);
	cout << BASS_ErrorGetCode() << endl;

	soundfonts[1] = BASS_MIDI_FontInit(string(resources_path + "MSB bank.sf2").c_str(), BASS_MIDI_FONT_XGDRUMS);
	cout << BASS_ErrorGetCode() << endl;
	//BASS_MIDI_FontSetVolume(soundfonts[1], 0.5f);
	//cout << BASS_ErrorGetCode() << endl;

	soundfonts[2] = BASS_MIDI_FontInit(string(resources_path + "MT-32.sf2").c_str(), NULL);
	cout << BASS_ErrorGetCode() << endl;
	//BASS_MIDI_FontSetVolume(soundfonts[2], 0.5f);
	//cout << BASS_ErrorGetCode() << endl;

	BASS_MIDI_FONTEX fontex;

	for (int b = 1; b <= 127; b++)
	{
		fontex.font = soundfonts[0];
		fontex.spreset = -1;
		fontex.sbank = b;
		fontex.dpreset = -1;
		fontex.dbank = 0;
		fontex.dbanklsb = b;
		font_info.push_back(fontex);
	}

	fontex.font = soundfonts[1];
	fontex.spreset = -1;
	fontex.sbank = -1;
	fontex.dpreset = -1;
	fontex.dbank = 0;
	fontex.dbanklsb = 0;
	font_info.push_back(fontex);

	fontex.font = soundfonts[2];
	fontex.spreset = -1;
	fontex.sbank = -1;
	fontex.dpreset = -1;
	fontex.dbank = 0;
	fontex.dbanklsb = 0;
	font_info.push_back(fontex);

	BASS_MIDI_StreamSetFonts(NULL, font_info.data(), (DWORD)font_info.size() | BASS_MIDI_FONT_EX);
	cout << BASS_ErrorGetCode() << endl;

	stream = BASS_MIDI_StreamCreateFile(false, argv[1], NULL, NULL,
		BASS_SAMPLE_LOOP | BASS_MIDI_DECAYEND | BASS_MIDI_DECAYSEEK | BASS_SAMPLE_FLOAT, NULL);
	cout << BASS_ErrorGetCode() << endl;
	BASS_ChannelSetDSP(stream, filter20Hz_f, NULL, 0);
	cout << BASS_ErrorGetCode() << endl;
	BASS_ChannelPlay(stream, false);
	cout << BASS_ErrorGetCode() << endl;
	system("PAUSE");
	return 0;
}

// Uruchomienie programu: Ctrl + F5 lub menu Debugowanie > Uruchom bez debugowania
// Debugowanie programu: F5 lub menu Debugowanie > Rozpocznij debugowanie

// Porady dotyczące rozpoczynania pracy:
//   1. Użyj okna Eksploratora rozwiązań, aby dodać pliki i zarządzać nimi
//   2. Użyj okna programu Team Explorer, aby nawiązać połączenie z kontrolą źródła
//   3. Użyj okna Dane wyjściowe, aby sprawdzić dane wyjściowe kompilacji i inne komunikaty
//   4. Użyj okna Lista błędów, aby zobaczyć błędy
//   5. Wybierz pozycję Projekt > Dodaj nowy element, aby utworzyć nowe pliki kodu, lub wybierz pozycję Projekt > Dodaj istniejący element, aby dodać istniejące pliku kodu do projektu
//   6. Aby w przyszłości ponownie otworzyć ten projekt, przejdź do pozycji Plik > Otwórz > Projekt i wybierz plik sln
