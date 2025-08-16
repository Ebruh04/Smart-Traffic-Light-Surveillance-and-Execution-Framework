// main.cpp
#include <raylib.h>
#include <vector>
#include <memory>
#include <string>

// Undefine Raylib color macros in case of conflicts with enums.
#undef RED
#undef GREEN
#undef YELLOW

// Color constants
const Color COLOR_RED            = {230, 41, 55, 255};
const Color COLOR_YELLOW         = {253, 249, 0, 255};
const Color COLOR_GREEN          = {0, 228, 48, 255};
const Color COLOR_PEDESTRIAN     = {0, 70, 0, 255};   // Very dark green for pedestrian mode
const Color COLOR_PEDESTRIAN_FAULT = {0, 100, 255, 255}; // Blue fallback when pedestrian light faulty
const Color COLOR_DARKGRAY       = DARKGRAY;
const Color COLOR_LIGHTGRAY      = LIGHTGRAY;
const Color COLOR_WHITE          = WHITE;
const Color COLOR_BLACK          = BLACK;
const Color COLOR_RAYWHITE       = RAYWHITE;
const Color COLOR_DARKGRAY_TEXT  = {80, 80, 80, 255};

enum class TLState { RED, YELLOW, GREEN, PEDESTRIAN, OFF };

class TrafficLight {
public:
    TrafficLight(const std::string& name = "", int greenMs = 10000, int yellowMs = 3000, int redMs = 13000)
        : m_name(name), m_state(TLState::RED), m_greenMs(greenMs),
          m_yellowMs(yellowMs), m_redMs(redMs), m_timeLeftMs(redMs), m_healthy(true) {}

    const std::string& getName() const { return m_name; }
    TLState getState() const { return m_state; }
    void setState(TLState s, int timeMs) {
        if (!m_healthy) { m_state = TLState::OFF; return; }
        m_state = s;
        m_timeLeftMs = timeMs;
    }
    bool isHealthy() const { return m_healthy; }
    void setHealthy(bool ok) {
        m_healthy = ok;
        if (!ok) m_state = TLState::OFF;
    }

    int timeLeft() const { return m_timeLeftMs; }
    void tick(int dt) {
        if (m_state != TLState::OFF) m_timeLeftMs -= dt;
        if (m_timeLeftMs < 0) m_timeLeftMs = 0;
    }

    int greenMs() const { return m_greenMs; }
    int yellowMs() const { return m_yellowMs; }
    int redMs() const { return m_redMs; }

private:
    std::string m_name;
    TLState m_state;
    int m_greenMs, m_yellowMs, m_redMs;
    int m_timeLeftMs;
    bool m_healthy;
};

class IntersectionController {
public:
    IntersectionController()
        : m_running(false), m_currentIdx(0), m_phase(0),
          m_phaseTimeLeft(0), m_pedestrianMode(false), m_pedestrianTime(0),
          m_savedIdx(0), m_savedPhase(0), m_savedTimeLeft(0), m_pedestrianTarget(-1)
    {
        m_poles.resize(4);
        m_poles[0] = std::make_shared<TrafficLight>("North");
        m_poles[1] = std::make_shared<TrafficLight>("East");
        m_poles[2] = std::make_shared<TrafficLight>("South");
        m_poles[3] = std::make_shared<TrafficLight>("West");
    }

    void start() {
        if (m_running) return;
        m_running = true;
        m_currentIdx = 0;
        m_phase = 0;
        int g = m_poles[m_currentIdx]->greenMs();
        setOnlyGreen(m_currentIdx, g);
        m_phaseTimeLeft = g;
    }

    void stop() {
        m_running = false;
        for (auto& p : m_poles) p->setState(TLState::OFF, 0);
    }

    void update(int dt) {
        if (!m_running) return;

        int speedFactor = hasAnyFault() ? 2 : 1; // 🚀 speed up if any fault
        dt *= speedFactor;

        if (m_pedestrianMode) {
            m_pedestrianTime -= dt;

            for (int i = 0; i < 4; ++i) {
                if (!m_poles[i]->isHealthy()) continue;
                if (i == m_pedestrianTarget)
                    m_poles[i]->setState(TLState::PEDESTRIAN, m_pedestrianTime);
                else
                    m_poles[i]->setState(TLState::RED, m_pedestrianTime);
            }

            if (m_pedestrianTime <= 0) {
                m_pedestrianMode = false;
                m_currentIdx = m_savedIdx;
                m_phase = m_savedPhase;
                m_phaseTimeLeft = m_savedTimeLeft;
                restoreCycleState();
            }
            for (auto& p : m_poles) p->tick(dt);
            return;
        }

        m_phaseTimeLeft -= dt;
        for (auto& p : m_poles) p->tick(dt);

        if (m_phaseTimeLeft > 0) return;

        if (m_phase == 0) {
            m_phase = 1;
            int y = m_poles[m_currentIdx]->yellowMs();
            m_poles[m_currentIdx]->setState(TLState::YELLOW, y);
            for (int i = 0; i < 4; ++i) {
                if (i != m_currentIdx) m_poles[i]->setState(TLState::RED, y);
            }
            m_phaseTimeLeft = y;
        } else {
            m_phase = 0;
            do {
                m_currentIdx = (m_currentIdx + 1) % 4;
            } while (!m_poles[m_currentIdx]->isHealthy()); // skip faulty

            int g = m_poles[m_currentIdx]->greenMs();
            setOnlyGreen(m_currentIdx, g);
            m_phaseTimeLeft = g;
        }
    }

    void pressPedestrian(int index) {
        if (index < 0 || index >= 4) return;
        if (!m_poles[index]->isHealthy()) return; // skip if faulty
        if (m_pedestrianMode) return;

        m_pedestrianMode = true;
        m_pedestrianTime = 5000; // 5 seconds pedestrian mode
        m_pedestrianTarget = index;

        // Save state to restore after
        m_savedIdx = m_currentIdx;
        m_savedPhase = m_phase;
        m_savedTimeLeft = std::max(0, m_phaseTimeLeft);
    }

    void toggleFaulty(int index) {
        if (index < 0 || index >= 4) return;
        bool nowHealthy = !m_poles[index]->isHealthy();
        m_poles[index]->setHealthy(nowHealthy);
    }

    std::vector<std::shared_ptr<TrafficLight>> m_poles;

private:
    void setOnlyGreen(int idx, int greenMs) {
        for (int i = 0; i < 4; ++i) {
            if (!m_poles[i]->isHealthy()) continue;
            if (i == idx) m_poles[i]->setState(TLState::GREEN, greenMs);
            else m_poles[i]->setState(TLState::RED, greenMs + m_poles[idx]->yellowMs());
        }
    }

    void restoreCycleState() {
        if (m_phase == 0) {
            setOnlyGreen(m_currentIdx, m_phaseTimeLeft);
        } else {
            for (int i = 0; i < 4; ++i) {
                if (!m_poles[i]->isHealthy()) continue;
                if (i == m_currentIdx) m_poles[i]->setState(TLState::YELLOW, m_phaseTimeLeft);
                else m_poles[i]->setState(TLState::RED, m_phaseTimeLeft);
            }
        }
    }

    bool hasAnyFault() {
        for (auto& p : m_poles)
            if (!p->isHealthy()) return true;
        return false;
    }

private:
    bool m_running;
    int m_currentIdx;
    int m_phase;
    int m_phaseTimeLeft;

    bool m_pedestrianMode;
    int m_pedestrianTime;
    int m_pedestrianTarget;

    int m_savedIdx;
    int m_savedPhase;
    int m_savedTimeLeft;
};

std::string msToStr(int ms) {
    if (ms < 1000) return std::to_string(ms) + "ms";
    int s = ms / 1000;
    return std::to_string(s) + "s";
}

int main() {
    const int screenW = 1000;
    const int screenH = 700;
    InitWindow(screenW, screenH, "Smart Traffic Light - Pedestrian + Faulty Mode");
    SetTargetFPS(60);

    IntersectionController controller;
    controller.start();

    const int boxW = 140;
    const int boxH = 260;
    Rectangle lightBoxes[4] = {
        { (screenW - boxW) / 2.0f, 40.f, (float)boxW, (float)boxH },
        { (float)(screenW - boxW - 40), (screenH - boxH) / 2.0f, (float)boxW, (float)boxH },
        { (screenW - boxW) / 2.0f, (float)(screenH - boxH - 40), (float)boxW, (float)boxH },
        { 40.f, (screenH - boxH) / 2.0f, (float)boxW, (float)boxH }
    };

    std::vector<Rectangle> pedButtons(4);
    for (int i = 0; i < 4; ++i) {
        Rectangle r = lightBoxes[i];
        float bw = 80, bh = 28;
        pedButtons[i] = { r.x + (r.width - bw) / 2.0f, r.y + r.height + 8.0f, bw, bh };
    }

    while (!WindowShouldClose()) {
        int dt = (int)(GetFrameTime() * 1000.0f);

        // Handle keys 1-4 to toggle faulty state
        if (IsKeyPressed(KEY_ONE)) controller.toggleFaulty(0);
        if (IsKeyPressed(KEY_TWO)) controller.toggleFaulty(1);
        if (IsKeyPressed(KEY_THREE)) controller.toggleFaulty(2);
        if (IsKeyPressed(KEY_FOUR)) controller.toggleFaulty(3);

        // Mouse press for pedestrian buttons
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 m = GetMousePosition();
            for (int i = 0; i < 4; ++i) {
                if (CheckCollisionPointRec(m, pedButtons[i])) {
                    controller.pressPedestrian(i);
                }
            }
        }

        controller.update(dt);

        BeginDrawing();
        ClearBackground(COLOR_RAYWHITE);

        auto lights = controller.m_poles;
        for (int i = 0; i < 4; ++i) {
            Rectangle r = lightBoxes[i];
            DrawRectangleRec(r, COLOR_DARKGRAY);

            float cx = r.x + r.width / 2.0f;
            float top = r.y + 20.0f;
            float spacing = 60.0f;
            float radius = 20.0f;

            TLState st = lights[i]->getState();

            Color bulbRed = (st == TLState::RED) ? COLOR_RED : COLOR_LIGHTGRAY;
            Color bulbYellow = (st == TLState::YELLOW) ? COLOR_YELLOW : COLOR_LIGHTGRAY;
            Color bulbGreen;

            if (st == TLState::GREEN) {
                bulbGreen = COLOR_GREEN;
            } else if (st == TLState::PEDESTRIAN) {
                bulbGreen = lights[i]->isHealthy() ? COLOR_PEDESTRIAN : COLOR_PEDESTRIAN_FAULT;
            } else {
                bulbGreen = COLOR_LIGHTGRAY;
            }

            DrawCircle((int)cx, (int)(top + 0 * spacing), (int)radius, bulbRed);
            DrawCircle((int)cx, (int)(top + 1 * spacing), (int)radius, bulbYellow);
            DrawCircle((int)cx, (int)(top + 2 * spacing), (int)radius, bulbGreen);

            DrawText(lights[i]->getName().c_str(), (int)r.x + 8, (int)(r.y + r.height - 28), 12, COLOR_WHITE);
            if (lights[i]->isHealthy())
                DrawText(msToStr(lights[i]->timeLeft()).c_str(), (int)r.x + (int)r.width - 50, (int)(r.y + r.height - 28), 12, COLOR_WHITE);
            else
                DrawText("FAULT", (int)r.x + (int)r.width - 55, (int)(r.y + r.height - 28), 12, COLOR_RED);

            // 🚦 Pedestrian button always visible
            Rectangle pb = pedButtons[i];
            Color btnColor = COLOR_LIGHTGRAY;
            DrawRectangleRec(pb, btnColor);
            DrawRectangleLines((int)pb.x, (int)pb.y, (int)pb.width, (int)pb.height, COLOR_BLACK);
            DrawText("PED", (int)pb.x + 18, (int)pb.y + 6, 12, COLOR_BLACK);
        }

        DrawText("Click PED to turn that light DARK GREEN (others RED for 5s)", 10, 10, 14, COLOR_DARKGRAY_TEXT);
        DrawText("Press 1-4 to toggle FAULTY state for North, East, South, West", 10, 30, 14, COLOR_DARKGRAY_TEXT);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
