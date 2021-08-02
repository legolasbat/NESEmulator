#pragma once

#include "Utils.h"
#include <vector>
#include <thread>
#include <future>

class APU
{
public:
	APU();
	void Clock();
	void CPUWrite(unWord dir, unByte b);
	unByte CPURead(unWord);

	float volume = 10.0f;

	bool isQueue = false;
	std::future<void> queue;

	std::vector<double> MixBuf;

private:
	void QueueAudio();

	void ClockSC1();
	void ClockSC2();
	void ClockSC3();
	void ClockSC4();

	std::vector<double> SC1Buf;
	std::vector<double> SC2Buf;
	std::vector<double> SC3Buf;
	std::vector<double> SC4Buf;

	double SC1Sample = 0.0;
	double SC1Output = 0.0;

	double globalTime = 0.0;

	uint32_t frameClockCounter = 0;
	uint32_t clockCounter = 0;

	bool modeCounter = false;

	int frames_per_sample = 18;

	bool useSC1 = false;
	bool useSC2 = false;
	bool useSC3 = false;
	bool useSC4 = false;

	bool SC1Halt = false;
	bool SC2Halt = false;

	float SC1amp = 0.0f;
	float SC2amp = 0.0f;

	float SC1freq = 0.0f;
	float SC2freq = 0.0f;

	unByte SC1dutyIndex = 0;
	unByte SC2dutyIndex = 0;

	int SC1pcc = frames_per_sample;
	int SC2pcc = frames_per_sample;

	unWord SC1timer = 0;
	unWord SC2timer = 0;


	float SC1envelopeVol = 0.0f;
	float SC2envelopeVol = 0.0f;

	float SC1constantVol = 15.0f;
	float SC2constantVol = 15.0f;

	bool SC1envelopeStart = false;
	bool SC2envelopeStart = false;

	unByte SC1envelope = 0;
	unByte SC2envelope = 0;

	unByte SC1envelopeDivider = 0;
	unByte SC2envelopeDivider = 0;

	struct sequencer {
		uint32_t sequence = 0;
		uint32_t newSequence = 0;
		unWord timer = 0;
		unWord reload = 0;
		unByte output = 0;

		unByte Clock(bool enable, std::function<void(uint32_t& s)> func) {
			if (enable) {
				timer--;
				if (timer == 0xffff) {
					timer = reload;
					func(sequence);
					output = sequence & 0x1;
				}
			}
			return output;
		}
	};

	sequencer SC1Seq;
	sequencer SC2Seq;

	struct oscpulse {
		double frequency = 0;
		double dutycycle = 0;
		double amplitude = 1;
		double pi = 3.14159;
		double harmonics = 20;

		double sample(double t) {
			double a = 0;
			double b = 0;
			double p = dutycycle * 2.0 * pi;

			auto approxsin = [](float t) {
				float j = t * 0.15915;
				j = j - (int)j;
				return 20.785 * j * (j - 0.5) * (j - 1.0f);
			};

			for (double n = 1; n < harmonics; n++) {
				double c = n * frequency * 2.0 * pi * t;
				a += -approxsin(c) / n;
				b += -approxsin(c - p * n) / n;

			}

			return (2.0 * amplitude / pi) * (a - b);
		}
	};

	oscpulse SC1Osc;
	oscpulse SC2Osc;

	struct sweeper {
		bool enabled = false;
		unByte period = 0;
		bool negate = false;
		unByte shift = 0;

		bool reload = false;
		unByte timer = 0;
		unWord change = 0;
		bool mute = false;

		void track(unWord& target) {
			if (enabled) {
				change = target >> shift;
				mute = (target < 8) || (target > 0x7ff);
			}
		}

		bool Clock(unWord& target, bool channel) {
			bool changed = false;

			if (timer == 0 && enabled && shift > 0 && !mute) {
				if (target >= 8 && change < 0x7ff) {
					if (negate) {
						target -= change - channel;
					}
					else {
						target += change;
					}
					changed = true;
				}
			}

			// if(enabled)
			{
				if (timer == 0 || reload) {
					timer = period;
					reload = false;
				}
				else
					timer--;

				mute = (target < 8) || (target > 0x7ff);
			}

			return changed;
		}
	};

	sweeper SC1Sweep;
	sweeper SC2Sweep;

	unByte SC1sweepDivider = 0;
	unByte SC2sweepDivider = 0;

	struct envelope {
		bool start = false;
		bool disable = false;

		uint16_t dividerCount = 0;
		uint16_t volume = 0;
		uint16_t output = 0;
		uint16_t decayCount = 0;

		void Clock(bool loop) {
			if (!start) {
				if (dividerCount == 0) {
					dividerCount = volume;

					if (decayCount == 0) {
						if (loop) {
							decayCount = 15;
						}
					}
					else
						decayCount--;
				}
				else
					dividerCount--;
			}
			else {
				start = false;
				decayCount = 15;
				dividerCount = volume;
			}

			if (disable) {
				output = volume;
			}
			else {
				output = decayCount;
			}
		}
	};

	envelope SC1Env;
	envelope SC2Env;

	struct lengthCounter {
		unByte counter = 0;

		unByte Clock(bool enable, bool halt) {
			if (!enable)
				counter = 0;
			else
				if (counter > 0 && !halt)
					counter--;
			return counter;
		}
	};

	lengthCounter SC1lc;
	lengthCounter SC2lc;

	unByte length_table[32] = { 10, 254, 20, 2, 40, 4, 80, 6,
							160, 8, 60, 10, 14, 12, 26, 14,
							12, 16, 24, 18, 48, 20, 96, 22,
							192, 24, 72, 26, 16, 28, 32, 30 };

	unByte duties[4][8] = {
	{0, 0, 0, 0, 0, 0, 0, 1 },
	{0, 0, 0, 0, 0, 0, 1, 1 },
	{0, 0, 0, 0, 1, 1, 1, 1 },
	{1, 1, 1, 1, 1, 1, 0, 0 }};

};

