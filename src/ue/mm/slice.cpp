//
// This file is a part of UERANSIM open source project.
// Copyright (c) 2021 ALİ GÜNGÖR.
//
// The software and all associated files are licensed under GPL-3.0
// and subject to the terms and conditions defined in LICENSE file.
//

#include "mm.hpp"
#include <nas/utils.hpp>
#include <ue/app/task.hpp>
#include <ue/sm/sm.hpp>

namespace nr::ue
{

static bool ContainsSNssai(const NetworkSlice &nssai, const SingleSlice &sNssai)
{
    return std::any_of(nssai.slices.begin(), nssai.slices.end(), [&sNssai](auto &i) { return i == sNssai; });
}

static void AppendSubset(const NetworkSlice &source, NetworkSlice &target, const NetworkSlice &rejectedInPlmn,
                         const NetworkSlice &rejectedInTa, size_t maxSNssai)
{
    size_t appended = 0;

    for (auto &slice : source.slices)
    {
        if (appended == maxSNssai)
            break;
        if (ContainsSNssai(rejectedInPlmn, slice))
            continue;
        if (ContainsSNssai(rejectedInTa, slice))
            continue;
        target.slices.push_back(slice);
        appended++;
    }
}

NetworkSlice NasMm::makeRequestedNssai(bool &isDefaultNssai) const
{
    isDefaultNssai = false;

    NetworkSlice res{};

    if (!m_usim->m_allowedNssai.slices.empty() || !m_usim->m_configuredNssai.slices.empty())
    {
        if (!m_usim->m_allowedNssai.slices.empty())
        {
            AppendSubset(m_usim->m_allowedNssai, res, m_usim->m_rejectedNssaiInPlmn, m_usim->m_rejectedNssaiInTa,
                         8);
            AppendSubset(m_usim->m_configuredNssai, res, m_usim->m_rejectedNssaiInPlmn,
                         m_usim->m_rejectedNssaiInTa, static_cast<size_t>(8) - res.slices.size());
        }
        else
        {
            AppendSubset(m_usim->m_configuredNssai, res, m_usim->m_rejectedNssaiInPlmn,
                         m_usim->m_rejectedNssaiInTa, 8);
        }
    }
    else if (!m_usim->m_defConfiguredNssai.slices.empty())
    {
        AppendSubset(m_usim->m_defConfiguredNssai, res, m_usim->m_rejectedNssaiInPlmn,
                     m_usim->m_rejectedNssaiInTa, 8);

        isDefaultNssai = true;
    }

    return res;
}

void NasMm::handleNetworkSlicingSubscriptionChange()
{
    // TODO
}

} // namespace nr::ue