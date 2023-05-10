#include <Arduino.h>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>

// Split a string into a vector of strings
// template <typename Out>
// void split(const std::string &s, char delim, Out result) {
//     std::istringstream iss(s);
//     std::string item;
//     while (std::getline(iss, item, delim)) {
//         *result++ = item;
//     }
// }

// std::vector<std::string> split(const std::string &s, char delim) {
//     std::vector<std::string> elems;
//     split(s, delim, std::back_inserter(elems));
//     return elems;
// }

const std::vector<std::string> split(const std::string str, char delimiter) {
    std::vector<std::string> temp;

    while(str.length() > 0) {
        const std::size_t index = str.find(delimiter);

        if (index == std::string::npos) {
            temp.push_back(str);
            break;
        }

        temp.push_back(str.substr(0, index));
        str.erase(0, index + 1);
    }

    return temp;
}

// Pretty print a vector of vectors
const std::string pprint(const std::vector<std::vector<int>>& vec) {
    std::string temp = "[";

    for (int i = 0; i < vec.size(); i++) {
        temp += "[";
        for (int j = 0; j < vec[i].size(); j++) {
            temp += std::to_string(vec[i][j]);
            if (j < vec[i].size() - 1) {
                temp += ", ";
            }
        }
        temp += "]";
        if (i < vec.size() - 1) {
            temp += ", ";
        }
    }

    temp += "]";

    return temp;
}

// Returns the index of the LED in the panel given the row and column
// LEDs in each panel are indexe
const int get_led_index(const int row, const int col, const int panel_width, const int panel_height) {
    // Assume the panels are vertically stacked
    // Determine which panel the LED is in
    const int panel = floor(row / panel_height);
    const int leds_per_panel = panel_width * panel_height;

    // Determine the row and column of the LED in the panel
    const int panel_row = row % panel_height;
    const int panel_col = col;

    // Determine the index of the LED in the panel
    const int panel_led_index = panel_height * panel_col + panel_row;

    // Determine the index of the LED in the strip
    return panel * leds_per_panel + panel_led_index;
}

// Unwrap a 1D array into a 2D array
template <typename T>
const std::vector<std::vector<T>> unwrap(const std::vector<T>& vec, const int width, const int height) {
    std::vector<std::vector<T>> temp;

    for (int i = 0; i < height; i++) {
        std::vector<T> row;
        for (int j = 0; j < width; j++) {
            row.push_back(vec[i * width + j]);
        }
        temp.push_back(row);
    }

    return temp;
}

const unsigned int Color(const byte r, const byte g, const byte b) {
  //Take the lowest 8 bits of each value and append them end to end
  return( (((unsigned int)b & 0xFF )<<16) | (((unsigned int)r & 0xFF)<<8) | ((unsigned int)g & 0xFF));
}
