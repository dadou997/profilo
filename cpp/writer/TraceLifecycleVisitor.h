// Copyright 2004-present Facebook. All Rights Reserved.

#pragma once

#include <deque>
#include <memory>
#include <utility>
#include <vector>
#include <errno.h>

#include <loom/entries/Entry.h>
#include <loom/entries/EntryParser.h>
#include <loom/writer/AbortReason.h>
#include <loom/writer/TraceCallbacks.h>

#include <zstr/zstr.hpp>

namespace facebook {
namespace loom {
namespace writer {

using namespace facebook::loom::entries;

class TraceLifecycleVisitor: public EntryVisitor {
public:

  // Timestamp precision is microsec by default.
  static const size_t kTimestampPrecision = 6;

  static const size_t kTraceFormatVersion = 3;

  TraceLifecycleVisitor(
    const std::string& folder,
    const std::string& trace_prefix,
    std::shared_ptr<TraceCallbacks> callbacks,
    const std::vector<std::pair<std::string, std::string>>& headers,
    int64_t trace_id);

  virtual void visit(const StandardEntry& entry) override;
  virtual void visit(const FramesEntry& entry) override;
  virtual void visit(const BytesEntry& entry) override;

  void abort(AbortReason reason);

  inline bool done() const {
    return done_;
  }

private:
  const std::string folder_;
  const std::string trace_prefix_;
  const std::vector<std::pair<std::string, std::string>> trace_headers_;
  std::unique_ptr<zstr::ofstream> output_;

  // chain of delegates
  std::deque<std::unique_ptr<EntryVisitor>> delegates_;
  int64_t expected_trace_;
  std::shared_ptr<TraceCallbacks> callbacks_;
  bool done_;

  inline bool hasDelegate() {
    return !delegates_.empty();
  }

  void onTraceStart(int64_t trace_id, int32_t flags);
  void onTraceAbort(int64_t trace_id, AbortReason reason);
  void onTraceEnd(int64_t trace_id);
  void cleanupState();
  void writeHeaders(std::ostream& output, std::string id);
};

} // namespace writer
} // namespace loom
} // namespace facebook