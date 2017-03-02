#ifndef BESS_PKTBATCH_H_
#define BESS_PKTBATCH_H_

#include <cassert>

#include <rte_memcpy.h>

namespace bess {

class Packet;

class PacketBatch {
 public:
  int cnt() const { return cnt_; }
  void set_cnt(int cnt) { cnt_ = cnt; }
  void incr_cnt(int n = 1) { cnt_ += n; }

  Packet *const *pkts() const { return pkts_; }
  Packet **pkts() { return pkts_; }

  void clear() { cnt_ = 0; }

  void add(Packet *pkt) { pkts_[cnt_++] = pkt; }

  bool empty() { return (cnt_ == 0); }

  bool full() { return (cnt_ == kMaxBurst); }

  void Copy(const PacketBatch *src) {
    int cnt = src->cnt_;

    cnt_ = cnt;
    rte_memcpy(reinterpret_cast<void *>(pkts_),
               reinterpret_cast<const void *>(src->pkts_),
               cnt * sizeof(Packet *));
  }

  static const size_t kMaxBurst = 32;

 private:
  int cnt_;
  Packet *pkts_[kMaxBurst];
};

static_assert(std::is_pod<PacketBatch>::value, "PacketBatch is not a POD Type");



class PacketQueue {
 public:
  PacketQueue():head(0),tail(0),cnt_(0){  }
  ~PacketQueue(){  }
  int cnt() const { return cnt_; }
  void clear() {
	  cnt_ = 0;
	  head=0;
	  tail=0;

  }


  bool enqueue(Packet *pkt){

	  if(full()){
		  return false;
	  }
	  pkts_[tail] = pkt;
	  tail=(tail+1)%kMaxBurst;
	  cnt_++;
	  return true;
  }


  bool dequeue(Packet *pkt){

	  if(empty()){
		  return false;
	  }
	  Packet * t= pkts_[head];
	  rte_memcpy(reinterpret_cast<void *>(pkt),
	             reinterpret_cast<const void *>(t),
	             sizeof(Packet));
	  head=(head+1)%kMaxBurst;
	  cnt_--;
	  return t;
  }

  bool empty() { return (cnt_ == 0); }

  bool full() { return (cnt_ == kMaxBurst); }

  static const size_t kMaxBurst = 32;

 private:
  int cnt_;
  int head;
  int tail;
  Packet *pkts_[kMaxBurst];
};

static_assert(std::is_pod<PacketQueue>::value, "PacketQueue is not a POD Type");

}  // namespace bess

#endif  // BESS_PKTBATCH_H_
