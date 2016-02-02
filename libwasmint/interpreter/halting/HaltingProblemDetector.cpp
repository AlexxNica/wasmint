/*
 * Copyright 2015 WebAssembly Community Group
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include "HaltingProblemDetector.h"
#include <stdexcept>

bool wasmint::HaltingProblemDetector::isLooping(InstructionCounter startCounter) {
    if (startCounter > vm_.instructionCounter()) {
        throw std::domain_error("startCounter is bigger than the current vm instruction counter");
    }

    bool result = false;
    const VMState backupState = vm_.state();

    InstructionCounter lastCounter = vm_.instructionCounter();

    while (true) {
        const MachinePatch& lastPatch = vm_.history().getCheckpoint(lastCounter);

        if (lastPatch.influencedByExternalState()) {
            throw CantMakeHaltingDecision("Patch indicates that its related state depend on the external state");
        }

        InstructionCounter nextRollbackCounter = lastPatch.startCounter();

        if (nextRollbackCounter < startCounter)
            nextRollbackCounter = startCounter;

        vm_.simulateTo(nextRollbackCounter);

        InstructionCounter counter = nextRollbackCounter;
        while (counter != lastCounter) {
            if (isIdentical(vm_.state(), backupState)) {
                result = true;
                break;
            }
            ++counter;
            vm_.simulateTo(counter);
        }
        if (!result) {
            lastCounter = nextRollbackCounter;
            if (lastCounter <= startCounter) {
                break;
            } else {
                --lastCounter;
            }
        } else {
            break;
        }
    }

    vm_.state() = backupState;

    return result;
}

bool wasmint::HaltingProblemDetector::isIdentical(const VMState& a, const VMState& b) {
    // we don't compare the instruction pointer on purpose
    // as we only compare for memory/thread equality when checking for reoccurring states

    if (a.heap() != b.heap())
        return false;
    if (a.thread() != b.thread())
        return false;
    return true;
}