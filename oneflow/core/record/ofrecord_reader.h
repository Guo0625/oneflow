#ifndef ONEFLOW_CORE_RECORD_OFRECORD_READER_H_
#define ONEFLOW_CORE_RECORD_OFRECORD_READER_H_

#include "oneflow/core/record/record.pb.h"
#include "oneflow/core/common/util.h"
#include "oneflow/core/persistence/in_stream.h"
#include "oneflow/core/common/buffer.h"

namespace oneflow {

struct OFRecordChunk {
  int64_t size = 0;
  std::unique_ptr<char[]> data;
};

class OFRecordReader {
 public:
  OF_DISALLOW_COPY_AND_MOVE(OFRecordReader);
  OFRecordReader() = default;
  virtual ~OFRecordReader() = default;

  virtual size_t Read(size_t n, OFRecord* allocated_records) = 0;
};

class NaiveOFRecordReader final : public OFRecordReader {
 public:
  OF_DISALLOW_COPY_AND_MOVE(NaiveOFRecordReader);
  explicit NaiveOFRecordReader(InStream* in) : NaiveOFRecordReader(in, GetMaxVal<size_t>()) {}
  NaiveOFRecordReader(InStream* in, size_t num_max_read);
  ~NaiveOFRecordReader() override = default;

 private:
  size_t Read(size_t n, OFRecord* allocated_records) override;

  InStream* in_stream_;
  size_t num_read_;
  const size_t num_max_read_;
};

class BufferedOFRecordReader final : public OFRecordReader {
 public:
  OF_DISALLOW_COPY_AND_MOVE(BufferedOFRecordReader);
  BufferedOFRecordReader(InStream* in, size_t buffer_size)
      : BufferedOFRecordReader(in, GetMaxVal<int64_t>(), buffer_size) {}
  BufferedOFRecordReader(InStream* in, size_t num_max_read, size_t buffer_size);
  ~BufferedOFRecordReader() override;

 private:
  size_t Read(size_t n, OFRecord* allocated_records) override;

  InStream* in_stream_;
  size_t num_read_;
  const size_t num_max_read_;
  const size_t buffer_size_;
  Buffer<std::shared_ptr<OFRecordChunk>> chunk_buffer_;
  std::thread reader_thread_;
};

class RandomShuffleOFRecordReader final : public OFRecordReader {
 public:
  OF_DISALLOW_COPY_AND_MOVE(RandomShuffleOFRecordReader);
  RandomShuffleOFRecordReader(InStream* in, size_t buffer_size, size_t num_max_read,
                              int32_t random_seed);
  RandomShuffleOFRecordReader(InStream* in, size_t buffer_size, size_t num_max_read)
      : RandomShuffleOFRecordReader(in, buffer_size, num_max_read, std::random_device()()) {}
  RandomShuffleOFRecordReader(InStream* in, size_t buffer_size)
      : RandomShuffleOFRecordReader(in, buffer_size, GetMaxVal<size_t>()) {}
  ~RandomShuffleOFRecordReader() override = default;

 private:
  size_t Read(size_t n, OFRecord* allocated_records) override;
  void FillBuffer();

  InStream* in_stream_;
  const size_t buffer_size_;
  const size_t num_max_read_;
  std::mt19937 random_gen_;
  size_t num_read_;
  std::vector<OFRecordChunk> buffered_chunks_;
  bool is_eof_;
};

}  // namespace oneflow

#endif  // ONEFLOW_CORE_RECORD_OFRECORD_READER_H_
