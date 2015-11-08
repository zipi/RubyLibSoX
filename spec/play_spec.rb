require 'libsox'

context 'example 3' do
  # on a system with a natural output channel like alsa
  # play a file handling changes to rate and channel count
  # with the rate and channels effects

  it 'plays a file with effects' do
    input = LibSoX.open_read './spec/fixtures/make_my_day.wav'
    output = LibSoX.open_write 'default', input.signal, nil, LibSoX.play_filetype
    osig = output.signal
    chain = LibSoX.new_chain input, output
    chain.dump_signal "osig", osig
    interum_signal = input.signal.dup
    chain.input_format = interum_signal
    chain.output_format = input.signal
    chain.add_effect 'input'
    puts "ao"
    chain.output_format = osig
    input.signal.rate != output.signal.rate && chain.add_effect('rate') && puts("set rate")
    puts "yo"
    chain.dump_signal "input", input.signal
    chain.dump_signal "in", chain.input_format
    chain.dump_signal "out", chain.output_format
    puts "bo"
    input.signal.channels != output.signal.channels && chain.add_effect('channels') && puts("set chanels")
    chain.add_effect 'output'
    puts "do"
    expect(chain.flow).to eq SOX_SUCCESS
=begin






    puts 'putttt'
    chain.add_effect 'input'
    #chain.add_effect "trim", "2"
    chain.dump_signal "input", chain.input_format
    chain.dump_signal "output", chain.output_format
    input.signal.rate != output.signal.rate && chain.add_effect('rate') && puts("set rate")
    puts 'go'
    chain.dump_signal "output", chain.output_format
    chain.dump_signal "input", chain.input_format
    input.signal.channels != output.signal.channels && chain.add_effect('channels', "#{output.signal.channels}") && puts("set chanels")
    puts 'yo'
    chain.dump_signal "output",  chain.output_format
    #chain.add_effect 'earwax'
    chain.add_effect 'output'
    chain.dump_signal "output", chain.output_format
    expect(chain.flow).to eq SOX_SUCCESS
=end
  end
end

