#pragma once

#include <catboost/libs/helpers/multiclass_label_helpers/visible_label_helper.h>

#include <catboost/libs/options/enums.h>
#include <catboost/libs/data_util/path_with_scheme.h>
#include <catboost/libs/data_util/line_data_reader.h>
#include <catboost/libs/column_description/column.h>
#include <catboost/libs/data/pool.h>
#include <library/threading/local_executor/local_executor.h>

#include <util/string/builder.h>
#include <util/generic/vector.h>
#include <util/generic/map.h>
#include <util/generic/maybe.h>
#include <util/generic/string.h>
#include <util/stream/output.h>
#include <util/stream/file.h>
#include <util/string/iterator.h>
#include <util/string/cast.h>

void CalcSoftmax(const TVector<double>& approx, TVector<double>* softmax);

TVector<double> CalcSigmoid(const TVector<double>& approx);

TVector<TVector<double>> PrepareEval(
    const EPredictionType predictionType,
    const TVector<TVector<double>>& approx,
    NPar::TLocalExecutor* localExecutor);

TVector<TVector<double>> PrepareEval(
    const EPredictionType predictionType,
    const TVector<TVector<double>>& approx,
    int threadCount);

void ValidateColumnOutput(const TVector<TString>& outputColumns,
                          const TPool& pool,
                          bool isPartOfFullTestSet=false,
                          bool CV_mode=false);

class TEvalResult {
public:
    TEvalResult() {
        RawValues.resize(1);
    }

    TVector<TVector<TVector<double>>>& GetRawValuesRef();
    void ClearRawValues();

    /// *Move* data from `rawValues` to `RawValues[0]`
    void SetRawValuesByMove(TVector<TVector<double>>& rawValues);

    void OutputToFile(
        NPar::TLocalExecutor* executor,
        const TVector<TString>& outputColumns,
        const TVisibleLabelsHelper& visibleLabelsHelper,
        const TPool& pool,
        bool isPartOfTestSet, // pool is a part of test set, can't output testSetPath columns
        IOutputStream* outputStream,
        const NCB::TPathWithScheme& testSetPath,
        std::pair<int, int> testFileWhichOf,
        const NCB::TDsvFormatOptions& testSetFormat,
        bool writeHeader = true,
        TMaybe<std::pair<size_t, size_t>> evalParameters = TMaybe<std::pair<size_t, size_t>>());
    void OutputToFile(
        int threadCount,
        const TVector<TString>& outputColumns,
        const TVisibleLabelsHelper& visibleLabelsHelper,
        const TPool& pool,
        bool isPartOfTestSet, // pool is a part of test set, can't output testSetPath columns
        IOutputStream* outputStream,
        const NCB::TPathWithScheme& testSetPath,
        std::pair<int, int> testFileWhichOf,
        const NCB::TDsvFormatOptions& testSetFormat,
        bool writeHeader = true);

private:
    TVector<TVector<TVector<double>>> RawValues; // [evalIter][dim][docIdx]
};
