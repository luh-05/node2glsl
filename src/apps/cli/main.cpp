
#include "blender/xml/xml.hpp"
#include "mollusk/evaluation/evaluator.hpp"
#include "mollusk/plugins/plugins.hpp"
#include <fstream>
#include <spdlog/spdlog.h>
#include <string>

#include <absl/flags/flag.h>
#include <absl/flags/parse.h>
#include <absl/flags/usage.h>

ABSL_FLAG(std::string, backend, "", "Backend to use");
ABSL_FLAG(std::string, in, "", "Filepath to read from");
ABSL_FLAG(std::string, out, "", "Filepath to write to");
ABSL_FLAG(std::string, log_level, "info", "logging level");

#define CHECK_OK(var, stmt)                                                    \
  auto var##_s = stmt;                                                         \
  if (!var##_s.ok()) {                                                         \
    spdlog::error(var##_s.status().ToString());                                \
    return 1;                                                                  \
  }                                                                            \
  auto var = var##_s.value();

int main(int argc, char *argv[]) {
  absl::SetProgramUsageMessage(
      "Simple backend for Mollusk\nUsage: mollusk_cli --backend [backend] --in "
      "[./myinput] --out [./myoutput]");
  absl::ParseCommandLine(argc, argv);

  auto backend = absl::GetFlag(FLAGS_backend);
  auto in_path = absl::GetFlag(FLAGS_in);
  auto out_path = absl::GetFlag(FLAGS_out);

  const std::string fll = absl::GetFlag(FLAGS_log_level);
  if (strcmp(fll.c_str(), "info") == 0) {
    spdlog::set_level(spdlog::level::info);
  } else if (strcmp(fll.c_str(), "debug") == 0) {
    spdlog::set_level(spdlog::level::debug);
  } else {
    spdlog::error("Log level '{}' not recognized. Defaulting to 'info'.", fll);
    spdlog::set_level(spdlog::level::info);
  }

  if (backend != "blender") {
    spdlog::error("Backend '{}' is not supported! available: [blender]",
                  backend);
    return -1;
  }

  if (in_path == "") {
    spdlog::error("--in must be set");
    return -1;
  }
  if (out_path == "") {
    spdlog::error("--out must be set");
    return -1;
  }

  spdlog::info("Loading '{}' plugin", backend);

  msk::PluginStore store;
  if (auto s = store.FetchPlugin(); !s.ok()) {
    spdlog::error(s.ToString());
    return 1;
  }

  spdlog::info("Reading input file '{}'", in_path);

  msk::blender::XMLParser parser(&store);

  absl::Status status = parser.XMLread(in_path);
  if (!status.ok()) {
    spdlog::error(status.ToString());
    return 1;
  }

  spdlog::info("Parsing graph 0");
  CHECK_OK(context, parser.ParseGraph("0"));

  context.get()->SetDefinitios(store.definitions);
  context.get()->SetCastPolicies(store.cast_policies);

  spdlog::info("Evaluating graph");

  msk::Evaluator eval(context,
                      std::make_unique<msk::ForwardEvaluationStrategy>(true));

  std::string res;
  if (auto s = eval.Evaluate(res); !s.ok()) {
    spdlog::error(s.ToString());
    return 1;
  }

  spdlog::info("Emitting glsl to '{}'", out_path);

  std::ofstream out_file(out_path);

  out_file << res;

  out_file.close();

  spdlog::info("Sucessfully Emitted GLSL to {}", out_path);
}
