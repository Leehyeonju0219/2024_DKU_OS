/*
*	DKU Operating System Lab
*	    Lab1 (Scheduler Algorithm Simulator)
*	    Student id : 
*	    Student name : 
*/

#include <string>
#include <stdio.h>
#include <iostream>
#include <queue>
#include <algorithm>
#include "sched.h"

class SPN : public Scheduler{
    private:
        /*
        * 구현 (멤버 변수/함수 추가 및 삭제 가능)
        */
    public:
        SPN(std::queue<Job> jobs, double switch_overhead) : Scheduler(jobs, switch_overhead) {
            name = "SPN";
            /*
            * 위 생성자 선언 및 이름 초기화 코드 수정하지 말것.
            * 나머지는 자유롭게 수정 및 작성 가능
            */
        }

        int run() override {
            // 현재 진행 중인 작업이 없고, 작업 대기열에 작업이 있는 경우
            if (current_job_.name == 0 && !job_queue_.empty()) {
                // 대기열에서 가장 짧은 서비스 시간을 가진 작업을 찾음
                auto shortest = std::min_element(job_queue_.begin(), job_queue_.end(), [](const Job& a, const Job& b) {
                    return a.service_time < b.service_time; 
                });
                
                // 가장 짧은 작업을 현재 작업으로 설정
                current_job_ = *shortest;
                job_queue_.erase(shortest); // 해당 작업을 대기열에서 제거
            }
            
            if (current_job_.name != 0) {
                // 현재 작업의 남은 시간을 감소시킴
                current_job_.remain_time--;
                // 작업이 완료된 경우
                if (current_job_.remain_time == 0) {
                    end_jobs_.push_back(current_job_); // 완료된 작업 목록에 추가
                    current_job_ = Job(); // 현재 작업을 리셋
                }
            }

            // 대기열과 현재 진행 중인 작업이 모두 없는 경우 -1 반환
            return job_queue_.empty() && current_job_.name == 0 ? -1 : current_job_.name;
        }
};

class RR : public Scheduler{
    private:
        int time_slice_;
        int left_slice_;
        std::queue<Job> waiting_queue;
        /*
        * 구현 (멤버 변수/함수 추가 및 삭제 가능)
        */
    public:
        RR(std::queue<Job> jobs, double switch_overhead, int time_slice) : Scheduler(jobs, switch_overhead) {
            name = "RR_"+std::to_string(time_slice);
            /*
            * 위 생성자 선언 및 이름 초기화 코드 수정하지 말것.
            * 나머지는 자유롭게 수정 및 작성 가능 (아래 코드 수정 및 삭제 가능)
            */
            time_slice_ = time_slice; 
            left_slice_ = time_slice;
        }

        int run() override {
            if (current_job_.name == 0 && !job_queue_.empty()) {
                current_job_ = job_queue_.front();
                job_queue_.pop();
                left_slice_ = time_slice_; // 타임 슬라이스 초기화
            }

            if (current_job_.name != 0) {
                current_job_.remain_time--;
                left_slice_--;

                // 작업이 완료되거나 타임 슬라이스가 소진된 경우
                if (current_job_.remain_time == 0 || left_slice_ == 0) {
                    if (current_job_.remain_time > 0) {
                        // 작업이 아직 완료되지 않았다면, 다시 대기열에 추가
                        job_queue_.push(current_job_);
                    } else {
                        // 작업이 완료된 경우, 완료 목록에 추가
                        end_jobs_.push_back(current_job_);
                    }
                    current_job_ = Job(); // 현재 작업 리셋
                    if (!job_queue_.empty()) {
                        // 다음 작업 선택
                        current_job_ = job_queue_.front();
                        job_queue_.pop();
                        left_slice_ = time_slice_; // 타임 슬라이스 초기화
                    }
                }
            }

            // 다음 타임 슬롯으로 이동
            current_time_++;
            // 실행할 작업이 더 이상 없으면 -1 반환
            return current_job_.name == 0 && job_queue_.empty() ? -1 : current_job_.name;
        }             
};

class SRT : public Scheduler{
    private:
        /*
        * 구현 (멤버 변수/함수 추가 및 삭제 가능)
        */
    public:
        SRT(std::queue<Job> jobs, double switch_overhead) : Scheduler(jobs, switch_overhead) {
            name = "SRT";
            /*
            * 위 생성자 선언 및 이름 초기화 코드 수정하지 말것.
            * 나머지는 자유롭게 수정 및 작성 가능
            */
        }

        int run() override {
            // 현재 작업이 없고 대기열에 작업이 있는 경우, 가장 짧은 남은 시간을 가진 작업을 찾아 선택
            if (current_job_.name == 0 || (!job_queue_.empty() && job_queue_.front().remain_time < current_job_.remain_time)) {
                if (current_job_.name != 0) {
                    // 현재 진행 중인 작업이 있고, 대기열의 작업이 더 짧은 경우, 현재 작업을 대기열로 다시 넣음
                    job_queue_.push(current_job_);
                }
        
                // 대기열에서 가장 짧은 남은 시간을 가진 작업을 선택
                current_job_ = *std::min_element(job_queue_.begin(), job_queue_.end(), [](const Job& a, const Job& b) {
                    return a.remain_time < b.remain_time;
                });
                job_queue_.erase(std::remove_if(job_queue_.begin(), job_queue_.end(), [&](const Job& job){
                    return job.name == current_job_.name;
                }), job_queue_.end());
            }

            // 선택된 작업을 실행
            if (current_job_.name != 0) {
                current_job_.remain_time--;
                if (current_job_.remain_time == 0) {
                    // 작업이 완료되면, 완료 목록에 추가
                    end_jobs_.push_back(current_job_);
                    current_job_ = Job(); // 현재 작업 리셋
                }
            }

            // 시간 업데이트 및 다음 작업으로 이동
            current_time_++;
            return current_job_.name == 0 ? -1 : current_job_.name;
        }
};

class HRRN : public Scheduler{
    private:
        /*
        * 구현 (멤버 변수/함수 추가 및 삭제 가능)
        */    
    public:
        HRRN(std::queue<Job> jobs, double switch_overhead) : Scheduler(jobs, switch_overhead) {
            name = "HRRN";
            /*
            * 위 생성자 선언 및 이름 초기화 코드 수정하지 말것.
            * 나머지는 자유롭게 수정 및 작성 가능
            */            
        }

        int run() override {
            /*
            구현 
            */
            return -1;
        }
};

// FeedBack 스케줄러 (queue 개수 : 4 / boosting 없음)
class FeedBack : public Scheduler{
    private:
        /*
        * 구현 (멤버 변수/함수 추가 및 삭제 가능)
        */    
    public:
        FeedBack(std::queue<Job> jobs, double switch_overhead, bool is_2i) : Scheduler(jobs, switch_overhead) {
            if(is_2i){
                name = "FeedBack_2i";
            } else {
                name = "FeedBack_1";
            }

            /*
            * 위 생성자 선언 및 이름 초기화 코드 수정하지 말것.
            * 나머지는 자유롭게 수정 및 작성 가능
            */
        }

        int run() override {
            /*
            * 구현 
            */
            return -1;
        }
};
