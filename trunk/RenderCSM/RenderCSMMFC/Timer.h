#ifndef __TIMER_H__
#define __TIMER_H__

class Timer
{
public:
	/*! \brief ���캯�� */
	Timer();

	/*! \brief ��ʼ��ʱ�� */
	void Start();

	/*! \brief ֹͣ��ʱ�� */
	void Stop();

	/*! \brief ���¼�ʱ��\n
	    �������ϴθ��º����ȥʱ�䣬����FPS��������ʱ�� */
	void Update();

	BOOL IsStopped() { return m_timerStopped; }
	float GetFPS() { return m_fps; }
	float GetRunningTime() { return m_runningTime; }
	float GetElapsedTime() { return m_timerStopped ? 0.0f : m_timeElapsed; }

private:
	INT64 m_ticksPerSecond;
	INT64 m_currentTime;
	INT64 m_lastTime;
	INT64 m_lastFPSUpdate;
	INT64 m_FPSUpdateInterval;
	UINT m_numFrames;
	float m_runningTime;
	float m_timeElapsed;
	float m_fps;
	BOOL m_timerStopped;		//timer�Ƿ�ֹͣ
};
#endif