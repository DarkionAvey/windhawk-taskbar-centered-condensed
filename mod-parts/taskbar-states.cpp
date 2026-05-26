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
  float lastStartButtonXActual=0.0f;
  float lastRootWidth=0.0f;
  float lastTargetTaskFrameOffsetX=0.0f;
  float lastLeftMostEdgeTray{0};
  int lastRightMostEdgeTray{0};
  // Cached SetWindowRgn inputs (in DIPs) so we only call SetWindowRgn when the
  // visible bounds actually change. lastRegionClear=true means the window is
  // currently set to no region (i.e. full-width clickable).
  float lastRegionX{-1.0f};
  float lastRegionW{-1.0f};
  float lastRegionCorner{-1.0f};
  bool lastRegionClear{true};
};
static std::unordered_map<std::wstring, TaskbarState> g_taskbarStates;
