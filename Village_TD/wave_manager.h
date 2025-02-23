class WaveManager : public Manager<WaveManager>
{
	friend class Manager<WaveManager>;

public:
	void on_update(double delta) {
		static ConfigManager* config = ConfigManager::instance();
		static const std::vector<Wave>& waves = config->waves;

		if (config->is_game_over) return;

		if (!is_wave_started) {
			timer_start_wave.update(delta);
			return;
		}
		else {
			timer_spawn_enemy.update(delta);
		}

		if (is_spawned_last_enemy && EnemyManager::instance()->check_cleared()) {
			CoinManager::instance()->increase_coin(waves[idx_wave].rewards);
			idx_wave++;

			if (idx_wave >= waves.size()) {
				config->is_game_win = true;
				config->is_game_over = true;
				return;
			}
			else {
				idx_spawn_event = 0;
				is_wave_started = false; // Reset is_wave_started to false
				is_spawned_last_enemy = false;
				timer_start_wave.set_wait_time(waves[idx_wave].interval);
				timer_start_wave.restart(); // Restart the timer for the next wave
			}
		}
	}


	void reset() {
		idx_wave = 0;
		idx_spawn_event = 0;
		is_wave_started = false;
		is_spawned_last_enemy = false;
		timer_start_wave.set_wait_time(ConfigManager::instance()->waves[0].interval);
		timer_start_wave.restart();
	}

protected:
	WaveManager() {
		static const ConfigManager* config = ConfigManager::instance();
		static const std::vector<Wave>& waves = config->waves;

		timer_start_wave.set_one_shot(true);
		timer_start_wave.set_wait_time(waves[0].interval);
		timer_start_wave.set_on_timeout([this]() {
			is_wave_started = true;
			timer_spawn_enemy.set_wait_time(waves[idx_wave].spawn_events[0].interval);
			timer_spawn_enemy.restart();
			});

		timer_spawn_enemy.set_one_shot(true);
		timer_spawn_enemy.set_on_timeout([this]() {
			const std::vector<Wave::SpawnEvent>& spawn_events = waves[idx_wave].spawn_events;
			const Wave::SpawnEvent& spawn_event = spawn_events[idx_spawn_event];

			EnemyManager::instance()->spawn_enemy(spawn_event.enemy_type, spawn_event.spawn_point);

			idx_spawn_event++;
			if (idx_spawn_event >= spawn_events.size()) {
				is_spawned_last_enemy = true;
				return;
			}

			timer_spawn_enemy.set_wait_time(spawn_events[idx_spawn_event].interval);
			timer_spawn_enemy.restart();
			});
	};

	~WaveManager() = default;

private:
	int idx_wave = 0;
	int idx_spawn_event = 0;

	Timer timer_start_wave;
	Timer timer_spawn_enemy;

	bool is_wave_started = false;
	bool is_spawned_last_enemy = false;
};
