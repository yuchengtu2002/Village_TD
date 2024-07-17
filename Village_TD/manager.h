#pragma once
template <typename T>

class Manager {
	public:
		static T* instance() {
			
			if (!manager) {
				manager = new T();
			}
			return manager;
		}
		~Manager() =default;

	private:
		static Manager* manager;

	protected:
		Manager() = default;
		~Manager() = default;
		
		Manager(const Manager&) = delete;
		Manager& operator=(const Manager&) = delete;

};

