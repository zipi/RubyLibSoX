require 'libsox'

context 'example 3' do
  # on a system with a natural output channel like alsa
  # play a file handling changes to rate and channel count
  # with the rate and channels effects

  it 'plays a file with effects' do
    lib = LibSoX.new
    input = LibSoX.open_read './spec/fixtures/make_my_day.wav'
    output = LibSoX.open_write 'default', input.signal, nil, LibSoX.play_filetype
    chain = LibSoX.new_chain input, output
    interim_signal = input.signal.dup

    chain.input_format = interim_signal
    chain.output_format = input.signal
    chain.add_effect 'input'
    chain.add_effect 'trim', '1.5'

    chain.output_format = output.signal
    input.signal.rate != output.signal.rate && chain.add_effect('rate') && puts("set rate")
    input.signal.channels != output.signal.channels && chain.add_effect('channels') && puts("set chanels")

    interim_signal.length = output.signal.length
    chain.add_effect 'output'

    expect(chain.flow).to eq SOX_SUCCESS
  end
end

