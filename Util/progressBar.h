#ifndef PROGRESS_BAR_H
#define PROGRESS_BAR_H

#include <iostream>

class ProgressBar {

	int barWidth = 70;
public:
	ProgressBar(int barWidth) {
		this->barWidth = barWidth;
	}

	void updateProgressBar(float progress) {
        std::cout << "[";
        int pos = barWidth * progress;
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "] " << int(progress * 100.0) << " %\r";
        std::cout.flush();
        if (progress == 1.0f)
            std::cout << std::endl;
	}
};

#endif