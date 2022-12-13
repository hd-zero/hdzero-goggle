#!/bin/sh

function amixer_adc()
{
        onoff=$1
        amixer cset name='AIF1 AD0L Mixer ADCL Switch' $onoff
        amixer cset name='AIF1 AD0R Mixer ADCR Switch' $onoff        
}

function amixer_mic1()
{
        onoff=$1
        amixer cset name='LADC input Mixer MIC1 boost Switch' $onoff
        amixer cset name='RADC input Mixer MIC1 boost Switch' $onoff
}

function amixer_mic2()
{
        onoff=$1
        amixer cset name='LADC input Mixer MIC2 boost Switch' $onoff
        amixer cset name='RADC input Mixer MIC2 boost Switch' $onoff
}

function amixer_linein()
{
        onoff=$1
        amixer cset name='LADC input Mixer LINEINL Switch' $onoff
        amixer cset name='RADC input Mixer LINEINR Switch' $onoff
}

function amixer_clear()
{
        amixer cset name='AIF1 AD0L Mixer ADCL Switch' 0
        amixer cset name='AIF1 AD0R Mixer ADCR Switch' 0
        amixer cset name='LADC input Mixer MIC1 boost Switch' 0
        amixer cset name='RADC input Mixer MIC1 boost Switch' 0
        amixer cset name='LADC input Mixer MIC2 boost Switch' 0
        amixer cset name='RADC input Mixer MIC2 boost Switch' 0
        amixer cset name='LADC input Mixer LINEINL Switch' 0
        amixer cset name='RADC input Mixer LINEINR Switch' 0
}

function lineout_mixer_dac_switch()
{
        onoff=$1
        amixer cset name='Left Output Mixer DACL Switch' $onoff
        amixer cset name='Right Output Mixer DACR Switch' $onoff
}

function lineout_mixer_mic1_switch()
{
        onoff=$1
        amixer cset name='Left Output Mixer MIC1Booststage Switch' $onoff
        amixer cset name='Right Output Mixer MIC1Booststage Switch' $onoff
}

function lineout_mixer_mic2_switch()
{
        onoff=$1
        amixer cset name='Left Output Mixer MIC2Booststage Switch' $onoff
        amixer cset name='Right Output Mixer MIC2Booststage Switch' $onoff
}

function lineout_mixer_linein_switch()
{
        onoff=$1
        amixer cset name='Left Output Mixer LINEINL Switch' $onoff
        amixer cset name='Right Output Mixer LINEINR Switch' $onoff
}

function amixer_lineout()
{
        onoff=$1
        amixer cset name='Lineout Switch' $onoff
        amixer cset name='LINEOUTL Mux' $onoff
        amixer cset name='LINEOUTR Mux' $onoff
        #amixer cset name='DACL Mixer AIF1DA0L Switch' $onoff
        #amixer cset name='DACR Mixer AIF1DA0R Switch' $onoff
        #amixer cset name='Left Output Mixer DACL Switch' $onoff
        #amixer cset name='Right Output Mixer DACR Switch' $onoff
}

if [ $# != 1 ]
then
        echo "params count must equal 1"
        exit 1
fi

if [ $1 == "in_mic1" ]
then
        amixer_clear
        amixer_mic1 1
        amixer_adc 1
fi

if [ $1 == "in_mic2" ]
then
        amixer_clear
        amixer_mic2 1
        amixer_adc 1
fi

if [ $1 == "in_linein" ]
then
        amixer_clear
        amixer_linein 1
        amixer_adc 1
fi

if [ $1 == "out_on" ]
then
        amixer_lineout 1
        amixer cset name='lineout volume' 31
        amixer cset name='LINEINL/R to L_R output mixer gain' 7
fi

if [ $1 == "out_off" ]
then
        amixer_lineout 0
fi

if [ $1 == "out_dac_on" ]
then
        lineout_mixer_dac_switch 1
fi

if [ $1 == "out_dac_off" ]
then
        lineout_mixer_dac_switch 0
fi

if [ $1 == "out_mic1_on" ]
then
        lineout_mixer_mic1_switch 1
fi

if [ $1 == "out_mic1_off" ]
then
        lineout_mixer_mic1_switch 0
fi

if [ $1 == "out_mic2_on" ]
then
        lineout_mixer_mic2_switch 1
fi

if [ $1 == "out_mic2_off" ]
then
        lineout_mixer_mic2_switch 0
fi


if [ $1 == "out_linein_on" ]
then
        lineout_mixer_linein_switch 1
fi

if [ $1 == "out_linein_off" ]
then
        lineout_mixer_linein_switch 0
fi


