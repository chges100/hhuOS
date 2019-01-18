#include "AccessArrayThreadPriority.h"

AccessArrayThreadPriority::AccessArrayThreadPriority(uint8_t priorityCount) : ThreadPriority(priorityCount),
        accessArray(priorityCount * (priorityCount - 1) / 2u) {

    uint8_t tmp[priorityCount];

    for(uint8_t i = 0; i < priorityCount; i++) {
        tmp[i] = i;
    }

    uint8_t index = getMaxPriority();

    for(uint32_t i = 0; i < accessArray.length(); i++) {
        while(tmp[index] == 0) {
            index++;
            index %= priorityCount;
        }

        accessArray[i] = index;
        tmp[index]--;

        index++;
        index %= priorityCount;
    }
}

uint8_t AccessArrayThreadPriority::getNextPriority() {
    auto index = static_cast<uint8_t>(currentIndex++ % (accessArray.length() + 1));

    if(index == accessArray.length()) {
        return 0;
    }

    return accessArray[index];
}

void AccessArrayThreadPriority::reset() {
    currentIndex = 0;
}
