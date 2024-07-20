#pragma once
#include <functional>

class Timer {
public:
	Timer() {

	}

	~Timer() {

	};

	void restart() {
		pass_time = 0;
		shotted = false;
	}

	void set_wait_time(double val) {
		wait_time = val;
	}

	void set_one_shot(bool val) {
		one_shot = val;
	}

	void set_on_timeout(std::function<void()> on_timeout) {
		this->on_timeout = on_timeout;
	}

	void pause() {
		paused = true;
	}

	void resume() {
		paused = false;
	}

	void update(double delta_time) {
		if (paused) return;

		pass_time += delta_time;
		if (pass_time >= wait_time) {
			bool can_shot = (!one_shot || (one_shot && !shotted));
			shotted = true;

			if (can_shot && on_timeout) {
				on_timeout();
			}
			pass_time -= wait_time;

		}
	}


private:
	double pass_time = 0;
	double wait_time = 0;
	bool paused = false;
	bool shotted = false;
	bool one_shot = false;

	std::function<void()> on_timeout;

};  