#include "gain_apply/gain_apply_simple.h"

DEFINE_ALGORITHM_CONSTRUCTOR(GainApply, GainApplySimple, GainApplyConfiguration)

void GainApply::setChannelGain(int channel, float gain)
{
    static_cast<Implementation<GainApplySimple, GainApplyConfiguration> *>(pimpl.get())->algo.setChannelGain(channel, gain);
}

float GainApply::getChannelGain(int channel) const
{
    return static_cast<Implementation<GainApplySimple, GainApplyConfiguration> *>(pimpl.get())->algo.getChannelGain(channel);
}
