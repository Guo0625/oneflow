#include "oneflow/core/operator/operator.h"

namespace oneflow {

class ClipByValueOp final : public Operator {
 public:
  OF_DISALLOW_COPY_AND_MOVE(ClipByValueOp);
  ClipByValueOp() = default;
  ~ClipByValueOp() = default;

  void InitFromOpConf() override {
    CHECK(op_conf().has_clip_by_value_conf());
    EnrollInputBn("in");
    EnrollTmpBn("clip_mask");
    EnrollOutputBn("out");
  }

  const PbMessage& GetCustomizedConf() const override {
    return this->op_conf().clip_by_value_conf();
  }

  Maybe<void> InferBlobDescs(std::function<BlobDesc*(const std::string&)> GetBlobDesc4BnInOp,
                             const ParallelContext* parallel_ctx, const SbpSignature* sbp_signature,
                             std::function<void(OpContext*)> EnrollOpCtx) const override {
    // input
    const BlobDesc* in = GetBlobDesc4BnInOp("in");
    // data_tmp: clip_mask
    BlobDesc* clip_mask = GetBlobDesc4BnInOp("clip_mask");
    *clip_mask = *in;
    clip_mask->set_data_type(kInt8);
    // output
    *GetBlobDesc4BnInOp("out") = *in;
    return Maybe<void>::Ok();
  }
};

REGISTER_OP(OperatorConf::kClipByValueConf, ClipByValueOp);

}  // namespace oneflow