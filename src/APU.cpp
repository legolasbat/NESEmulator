#include "APU.h"
#include "SDL.h"

#include <iostream>

#define PI2 6.28318530718

float fTime = 0;

SDL_AudioDeviceID audio_device;

static void callback(void* const userdata, Uint8* stream, int len) {
	Sint16* stream16 = (Sint16*)stream;

	int nb_samples = len / sizeof(Sint16);
	for (int i = 0; i < nb_samples; i++) {
		stream16[i] = 32000 * sin(fTime);

		fTime += 440.0 * PI2 / 44100.0;
		if (fTime >= PI2)
			fTime -= PI2;
	}
}

APU::APU() {
	// Audio
	SDL_setenv("SDL_AUDIODRIVER", "directsound", 1);

	SDL_Init(SDL_INIT_AUDIO);

	SDL_AudioSpec audio_spec = { 0 };
	audio_spec.freq = 44100;
	audio_spec.format = AUDIO_S16SYS;
	audio_spec.channels = 1;
	audio_spec.samples = 512;
	audio_spec.callback = NULL;
	audio_spec.userdata = NULL;

	audio_device = SDL_OpenAudioDevice(NULL, 0, &audio_spec, NULL, 0);

	SDL_PauseAudioDevice(audio_device, 0);
	// End audio
}

void APU::QueueAudio() {
	while (SDL_GetQueuedAudioSize(audio_device) > 4096 * 4) {}
}

std::thread queue;

void APU::Clock()
{
	//ClockSC1();
	//ClockSC2();
	//ClockSC3();
	//ClockSC4();

	bool quarterFrameClock = false;
	bool halfFrameClock = false;

	globalTime += (1.0 / 5369319.0);

	if (clockCounter % 6 == 0) {
		frameClockCounter++;

		if (frameClockCounter == 3729) {
			quarterFrameClock = true;
		}

		if (frameClockCounter == 7457) {
			quarterFrameClock = true;
			halfFrameClock = true;
		}

		if (frameClockCounter == 11186) {
			quarterFrameClock = true;
		}

		if (frameClockCounter == 14916) {
			quarterFrameClock = true;
			halfFrameClock = true;
			frameClockCounter = 0;
		}


		if (quarterFrameClock) {
			SC1Env.Clock(SC1Halt);
		}

		if (halfFrameClock) {
			SC1lc.Clock(useSC1, SC1Halt);


			SC1Sweep.Clock(SC1Seq.reload, 0);
		}

		// SC1 output

		SC1Osc.frequency = 1789773.0 / (16.0 * (double)(SC1Seq.reload + 1));
		SC1Osc.amplitude = (double)(SC1Env.output - 1) / 16.0;
		SC1Sample = SC1Osc.sample(globalTime);

		if (SC1lc.counter > 0 && SC1Seq.timer >= 8 && !SC1Sweep.mute && SC1Env.output > 2)
			SC1Output += (SC1Sample - SC1Output) * 0.5;
		else
			SC1Output = 0;


		if (!useSC1) SC1Output = 0;
	}

	SC1Sweep.track(SC1Seq.reload);

	SC1Buf.push_back(SC1Output);
	SC1Buf.push_back(SC1Output);

	clockCounter++;



	if (SC1Buf.size() >= 512/* && SC2Buf.size() >= 100 && SC3Buf.size() >= 100 && SC4Buf.size() >= 100*/) {
		for (int i = 0; i < 512; i++) {
			float res = 0;
			if (useSC1)
				res += SC1Buf.at(i) * volume;
			//if (useSC2)
			//	res += SC2Buf.at(i) * volume;
			//if (useSC3)
			//	res += SC3Buf.at(i) * volume;
			//if (useSC4)
			//	res += SC4Buf.at(i) * volume;

			//if(res != 0)
			//	std::cout << res * volume << std::endl;
			MixBuf.push_back(res);
		}
		//SDL_QueueAudio(audio_device, MixBuf.data(), MixBuf.size() * sizeof(float));

		SC1Buf.clear();
		SC2Buf.clear();
		SC3Buf.clear();
		SC4Buf.clear();
		MixBuf.clear();

		//queue = std::async(&APU::QueueAudio, this);
		//std::thread queue(&APU::QueueAudio, this);
		while (SDL_GetQueuedAudioSize(audio_device) > 4096 * sizeof(float)) {
			std::cout << SDL_GetQueuedAudioSize(audio_device) << std::endl;
		}
		//isQueue = true;
	}
}

void APU::ClockSC1() {

	

	/*if (((frameCounter == 3729 || frameCounter == 7457 || frameCounter == 11186 || frameCounter == 14915) && modeCounter)
		|| ((frameCounter == 3729 || frameCounter == 7457 || frameCounter == 11186 || frameCounter == 18641) && !modeCounter)){
		if (!SC1envelopeStart) {
			SC1envelopeDivider--;

			if (SC1envelopeDivider < 0) {
				SC1envelopeDivider = SC1Volume;
				if (SC1envelope > 0) {
					SC1envelope--;
					SC1envelopeVol = SC1envelope;
				}
				else {
					SC1envelopeVol = SC1envelope;
					if (SC1Halt) {
						SC1envelope = 15;
					}
				}
			}
		}
		else {
			SC1envelopeStart = false;
			SC1envelope = 15;
			SC1envelopeDivider = SC1Volume;
		}
	}
	if (SC1constantVolFlag) {
		SC1amp = SC1constantVol;
	}
	else {
		SC1amp = SC1envelopeVol;
	}

	if (frameCounter == 7456 || frameCounter == 14915) {
		if (SC1Halt == 0) {
			if (SC1lc.counter) {
				SC1lc.counter--;
			}
		}

		if (SC1Sweep.enabled) {
			SC1sweepDivider--;

			if (SC1sweepDivider < 0) {
				int16_t post = SC1timerTarget >> SC1Sweep.shift;
				int8_t neg = SC1Sweep.negate ? -1 : 1;
				int16_t sum = (uint16_t)(post * neg);
				SC1timerTarget = SC1timerTarget + sum;

				if (SC1timerTarget >= 0x7ff || SC1timerTarget <= 8) {
					SC1amp = 0;
					CPUWrite(0x4001, ((unWord)SC1Sweep.period << 4) & ((unWord)SC1Sweep.negate << 3) & (unWord)SC1Sweep.shift);
				}
			}
			if (SC1sweepDivider < 0 || SC1sweepReload) {
				//SC1sweepReload = false;
				SC1sweepDivider = SC1Sweep.period;
			}
		}
	}
	if (SC1lc.counter <= 0) {
		SC1enabled = false;
	}


	if (SC1timer <= 0) {
		SC1timer = SC1timerTarget;

		++SC1dutyIndex %= 8;
	}
	else {
		SC1timer--;
	}

	int duty = SC1Duty;
	if (duties[duty][SC1dutyIndex] == 1) {
		//std::cout << SC1amp << std::endl;
		SC1freq = SC1amp;
	}
	else
		SC1freq = 0;


	if (!--SC1pcc) {
		SC1pcc = frames_per_sample;

		if (SC1enabled && useSC1 && SC1lc.counter) {
			//std::cout << "input sample" << std::endl;
			SC1Buf.push_back((float)SC1freq / 100);
			SC1Buf.push_back((float)SC1freq / 100);
		}
		else {
			SC1Buf.push_back(0);
			SC1Buf.push_back(0);
		}
	}*/

}

void APU::ClockSC2() {

}

void APU::ClockSC3() {

}

void APU::ClockSC4() {

}

void APU::CPUWrite(unWord dir, unByte b) {
	switch (dir) {
	case 0x4000:
		switch ((b & 0xc0) >> 6) {
		case 0x00: SC1Seq.newSequence = 0b01000000; SC1Osc.dutycycle = 0.125; break;
		case 0x01: SC1Seq.newSequence = 0b01100000; SC1Osc.dutycycle = 0.250; break;
		case 0x02: SC1Seq.newSequence = 0b01111000; SC1Osc.dutycycle = 0.500; break;
		case 0x03: SC1Seq.newSequence = 0b10011111; SC1Osc.dutycycle = 0.750; break;
		}
		
		SC1Seq.sequence = SC1Seq.newSequence;
		SC1Halt = (b & 0x20);

		SC1Env.volume = (b & 0x0f);
		SC1Env.disable = (b & 0x10);

		break;

	case 0x4001:
		SC1Sweep.enabled = (b & 0x80);
		SC1Sweep.period = (b & 0x70) >> 4;
		SC1Sweep.negate = b & 0x08;
		SC1Sweep.shift = b & 0x07;
		SC1Sweep.reload = true;

		break;

	case 0x4002:
		SC1Seq.reload = (SC1Seq.reload & 0xff00) | b;

		break;

	case 0x4003:
		SC1Seq.reload = (unWord)(b & 0x07) << 8 | (SC1Seq.reload & 0x00ff);
		SC1Seq.timer = SC1Seq.reload;
		SC1Seq.sequence = SC1Seq.newSequence;

		SC1lc.counter = length_table[(b & 0xf8) >> 3];
		SC1Env.start = true;

		break;

	case 0x4005:
		SC2Sweep.enabled = (b & 0x80);
		SC2Sweep.period = (b & 0x70) >> 4;
		SC2Sweep.negate = b & 0x08;
		SC2Sweep.shift = b & 0x07;
		// Reload

		break;

	case 0x4015:
		useSC1 = b & 0x01;
		useSC2 = b & 0x02;
		useSC3 = b & 0x04;
		useSC4 = b & 0x08;

		// DMC stuff

		break;

	case 0x4017:
		modeCounter = (b >> 7);
		break;
	}
}

unByte APU::CPURead(unWord) {
	return 0;
}
