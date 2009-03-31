#ifndef __Analog_h__
#define __Analog_h__

void Analog(int multiplexChannel) {
    ADCSRA|=(1<<ADEN);
    ADMUX=multiplexChannel;
    ADMUX|=(1<<REFS0);
}

uint16_t adc_get_value() {
    ADCSRA|=(1<<ADSC);
    while(ADCSRA & (1<<ADSC));
    return ADCL+(ADCH<<8);
}

#endif
