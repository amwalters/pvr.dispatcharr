#pragma once

#include "hls_playlist.h"

#include <condition_variable>
#include <cstdint>
#include <fstream>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_set>

namespace dispatcharr { class Client; }

namespace dispatcharr::recording
{

class GrowingRecordedStream
{
public:
  explicit GrowingRecordedStream(Client& client);
  ~GrowingRecordedStream();

  GrowingRecordedStream(const GrowingRecordedStream&) = delete;
  GrowingRecordedStream& operator=(const GrowingRecordedStream&) = delete;

  static void CleanupStaleFiles(const std::string& directory);
  bool Open(int recordingId, const std::string& cacheDirectory);
  void Close();
  int Read(unsigned char* buffer, unsigned int size);
  int64_t Seek(int64_t offset, int whence);
  int64_t Length() const;
  int64_t DurationMicroseconds() const;
  bool IsOpen() const;

private:
  bool RefreshAndDownload(size_t maximumNewSegments = 0);
  bool AppendSegment(const HlsSegment& segment, const std::string& data);
  void Worker();

  Client& m_client;
  mutable std::mutex m_mutex;
  std::condition_variable m_changed;
  std::thread m_worker;
  std::ifstream m_consumer;
  std::ofstream m_producer;
  std::string m_cachePath;
  int m_recordingId = 0;
  int64_t m_position = 0;
  int64_t m_cachedBytes = 0;
  double m_cachedDuration = 0.0;
  int m_targetDuration = 6;
  bool m_open = false;
  bool m_closing = false;
  bool m_complete = false;
  bool m_failed = false;
  std::unordered_set<int64_t> m_downloadedSequences;
};

} // namespace dispatcharr::recording
