require 'libsox'

context 'example 0' do
  # with an input file
  # increase the volume and flang it with effects
  it 'chain style' do
    result = LibSoXEffectsChain.new.
      open_read('./spec/fixtures/good_bad_ugly.wav').
      add_effect('vol', '3dB').
      add_effect('flanger').
      open_write('./tmp/output.wav').
      flow_effects
    expect(result).to eq true
  end
end

context 'example 1' do
  # like example 0 
  # but with input_drain and output_flow funtions
  # that happen to read and write from files
  # but could do something else
end

context 'example 2' do
  start = 60
  period = 5
  sample = 0.1

  # open file, seek to start position,
  # read the period of sound in to a bufffer,
  # and then find the peak volume for each sample sized, sample, in the buffer
  # print the reduced information as a histogram
end

context 'example 3' do
  # on a system with a natural output channel like alsa
  # play a file handling changes to rate and channel count
  # with the rate and channels effects
end

context 'example 4' do
  # concatinate multiple input files
  # trunkate file on error
  # ensure all files have the same signal characteristics
  # using sox_read and sox_write
end

end

context 'example 5' do
  # read file into buffer first
  # process file and save to memory buffer
  # Then write it out
  # just to show sox_mem_read and sox_mem_write in action
end

context 'example 6' do
  # convert 2-channel 32-bit wav file
  # into a 1-channel 8-bit uLaw file
end
