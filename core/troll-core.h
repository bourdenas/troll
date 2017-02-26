#ifndef TROLL_CORE_TROLL_CORE_H_
#define TROLL_CORE_TROLL_CORE_H_

namespace troll {

class Core {
 public:
  static void Init();
  static void CleanUp();
  static void Run();

  Core() = delete;
  virtual ~Core() = delete;

  Core(const Core&) = delete;
  Core& operator=(const Core&) = delete;

 private:
  static bool InputHandling();
  static void RenderFrame();
  static void FrameStarted(int time_since_last_frame);
  static void FrameEnded(int time_since_last_frame);

  struct FpsCounter {
    int elapsed_time = 0;
    int fps = 0;
    int fp_count = 0;
  };
  static FpsCounter fps_counter_;
};

}  // namespace troll

#endif  // TROLL_CORE_TROLL_CORE_H_
