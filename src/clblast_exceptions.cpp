
// =================================================================================================
// This file is part of the CLBlast project. The project is licensed under Apache Version 2.0. This
// project loosely follows the Google C++ styleguide and uses a tab-size of two spaces and a max-
// width of 100 characters per line.
//
// Author(s):
//   Ivan Shapovalov <intelfx@intelfx.name>
//
// This file implements the exception hierarchy for CLBlast. It contains classes for exceptions
// generated by different parts of CLBlast (e.g. OpenCL API calls, internal logic, semantic BLAS
// errors).
//
// =================================================================================================

#include "clblast_exceptions.hpp"

namespace {
// =================================================================================================

std::string MakeReason(const std::string &reason, const std::string &subreason) {
  std::string r = reason;
  if (!subreason.empty()) {
    r += " (" + subreason + ")";
  }
  return r;
}

} // anonymous namespace

namespace clblast {
// =================================================================================================

BLASError::BLASError(StatusCode status, const std::string &subreason):
    ErrorCode(status,
              subreason,
              "BLAS error: " + MakeReason(std::to_string(static_cast<int>(status)), subreason)) {
}

RuntimeErrorCode::RuntimeErrorCode(StatusCode status, const std::string &subreason):
    ErrorCode(status,
              subreason,
              MakeReason(std::to_string(static_cast<int>(status)), subreason)) {
}

// =================================================================================================

StatusCode DispatchException()
{
  const char *message = nullptr;
  StatusCode status;

  try {
    throw;
  } catch (BLASError &e) {
    // no message is printed for invalid argument errors
    status = e.status();
  } catch (CLError &e) {
    message = e.what();
    status = static_cast<StatusCode>(e.status());
  } catch (RuntimeErrorCode &e) {
    message = e.what();
    status = e.status();
  } catch (Error<std::runtime_error> &e) {
    message = e.what();
    status = StatusCode::kUnknownError;
  }

  if (message) {
    fprintf(stderr, "CLBlast: %s\n", message);
  }
  return status;
}

// =================================================================================================

StatusCode DispatchExceptionForC()
{
  const char *message = nullptr;

  try {
    throw;
  } catch (std::exception &e) {
    message = e.what();
  } catch (...) {
    message = "unknown exception";
  }

  fprintf (stderr, "CLBlast (unexpected): %s\n", message);
  return StatusCode::kUnexpectedError;
}

// =================================================================================================

} // namespace clblast