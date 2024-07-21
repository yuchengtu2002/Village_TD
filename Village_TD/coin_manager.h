#pragma once
#include "manager.h"
#include "config_manager.h"
#include "resource_manager.h"

class CoinManager : public Manager<CoinManager>
{
	friend class Manager<CoinManager>;

public:
	void increase_coin(double val) {
		num_coin += val;
	}

	double get_current_coin() {
		return num_coin;
	}

	void decrease_coin(double val) {
		num_coin -= val;
		if (num_coin <= 0) {
			num_coin = 0;
		}
	}

protected:

private:
	double num_coin = 0;

};