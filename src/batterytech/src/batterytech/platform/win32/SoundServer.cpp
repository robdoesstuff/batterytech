#ifdef _WIN32

#include <windows.h>
#include "SoundServer.h"

// Internal WaveOut API callback function
static void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance,
		DWORD dwParam1, DWORD dwParam2) {
	if (uMsg == WOM_DONE) {
		CSoundServer *pServer = (CSoundServer*) dwInstance;
		if (pServer)
			pServer->fillNextBuffer();
	}
}

CSoundServer::CSoundServer() {
	m_pUserCallback = NULL;
	m_bServerRunning = FALSE;
	m_currentBuffer = 0;
	m_bIsFilling = FALSE;
}

CSoundServer::~CSoundServer() {
	close();
}

BOOL CSoundServer::open(USER_CALLBACK pUserCallback, long totalBufferedSoundLen) {
	m_pUserCallback = pUserCallback;
	m_bufferSize = ((totalBufferedSoundLen * REPLAY_RATE) / 1000)
			* REPLAY_SAMPLELEN * REPLAY_CHANNELS;
	m_bufferSize /= REPLAY_NBSOUNDBUFFER;

	WAVEFORMATEX wfx;
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = REPLAY_CHANNELS;
	wfx.nSamplesPerSec = REPLAY_RATE;
	wfx.wBitsPerSample = REPLAY_DEPTH;
	wfx.nBlockAlign = wfx.nChannels * REPLAY_SAMPLELEN;
	wfx.nAvgBytesPerSec = wfx.nBlockAlign + wfx.nSamplesPerSec;
	wfx.cbSize = 0;
	MMRESULT errCode = waveOutOpen(&m_hWaveOut, WAVE_MAPPER, &wfx,
			(DWORD) waveOutProc, (DWORD) this, // User data.
			(DWORD) CALLBACK_FUNCTION);

	if (errCode != MMSYSERR_NOERROR)
		return FALSE;
	int i;
	// Alloc the sample buffers.
	for (i = 0; i < REPLAY_NBSOUNDBUFFER; i++) {
		m_pSoundBuffer[i] = malloc(m_bufferSize);
		memset(&m_waveHeader[i], 0, sizeof(WAVEHDR));
	}

	// Fill all the sound buffers
	m_currentBuffer = 0;
	m_bServerRunning = TRUE;
	for (i = 0; i < REPLAY_NBSOUNDBUFFER; i++) {
		fillNextBuffer();
	}

	return TRUE;
}

void CSoundServer::close(void) {
	if (m_bServerRunning) {
		m_bServerRunning = FALSE;
		while (m_bIsFilling) {
			Sleep(1);
		}
		waveOutReset(m_hWaveOut);
		m_pUserCallback = NULL;
		for (int i = 0; i < REPLAY_NBSOUNDBUFFER; i++) {
			if (m_waveHeader[m_currentBuffer].dwFlags & WHDR_PREPARED) {
				waveOutUnprepareHeader(m_hWaveOut, &m_waveHeader[i],
						sizeof(WAVEHDR));
			}
			// TODO - why segfault?
			//free(m_pSoundBuffer[i]);
		}
		waveOutClose(m_hWaveOut);
	}
}

void CSoundServer::fillNextBuffer(void) {
	if (m_bServerRunning) {
		m_bIsFilling = TRUE;
		// check if the buffer is already prepared (should not !)
		if (m_waveHeader[m_currentBuffer].dwFlags & WHDR_PREPARED)
			waveOutUnprepareHeader(m_hWaveOut, &m_waveHeader[m_currentBuffer],
					sizeof(WAVEHDR));

		// Call the user function to fill the buffer
		if (m_pUserCallback) {
			m_pUserCallback(m_pSoundBuffer[m_currentBuffer], m_bufferSize);
		}
		// Prepare the buffer to be sent to the WaveOut API
		m_waveHeader[m_currentBuffer].lpData
				= (char*) m_pSoundBuffer[m_currentBuffer];
		m_waveHeader[m_currentBuffer].dwBufferLength = m_bufferSize;
		waveOutPrepareHeader(m_hWaveOut, &m_waveHeader[m_currentBuffer],
				sizeof(WAVEHDR));

		// Send the buffer the the WaveOut queue
		waveOutWrite(m_hWaveOut, &m_waveHeader[m_currentBuffer], sizeof(WAVEHDR));

		m_currentBuffer++;
		if (m_currentBuffer >= REPLAY_NBSOUNDBUFFER) {
			m_currentBuffer = 0;
		}
		m_bIsFilling = FALSE;
	}
}

#endif
