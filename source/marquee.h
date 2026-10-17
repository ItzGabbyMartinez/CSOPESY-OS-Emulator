#pragma once
#include <string>

// ============================================================
// FEATURE 2: MARQUEE DISPLAY & ANIMATION
// ============================================================
void startmarquee();
void stopmarquee();
void runmarquee();


// ============================================================
// FEATURE 3: MARQUEE SETTINGS
// ============================================================
void settext(const std::string& text);
void setspeed(int milliseconds);


// ============================================================
// FEATURE 4: PROGRAM INTEGRATION & PERFORMANCE
// ============================================================
void initializemarquee();
void shutdownmarquee();