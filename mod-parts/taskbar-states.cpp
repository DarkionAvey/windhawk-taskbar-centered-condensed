struct TaskbarState {
  std::chrono::steady_clock::time_point lastApplyStyleTime{};
  struct Data {
    int childrenCount;
    int rightMostEdge;
    unsigned int childrenWidth;
  } lastTaskbarData{};
  unsigned int lastChildrenWidthTaskbar{0};
  unsigned int lastTrayFrameWidth{0};
  float lastTargetWidth{0};
  float lastTargetOffsetX{0};
  float lastTargetOffsetY{0};
  float initOffsetX{-1};
  bool wasOverflowing{false};
  float lastStartButtonXCalculated=0.0f;
  float lastRootWidth=0.0f;
  float lastTargetTaskFrameOffsetX=0.0f;
  float lastLeftMostEdgeTray{0};
  int lastRightMostEdgeTray{0};
};
static std::unordered_map<std::wstring, TaskbarState> g_taskbarStates;
