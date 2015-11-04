require 'libsox'

describe 'Buffer extension' do
  before :each do
    @buffer = Buffer.new(512)
  end

  it 'should initialize an empty buffer' do
    expect(@buffer.raw_data.bytesize).to eq 2048
  end

  it 'should move data out of the buffer' do
    contents = @buffer.contents
    expect(contents.length).to be 512
    expect(contents[0]).to be 0
    expect(@buffer.raw_data.byteslice(0,4)).to eq "\x00\x00\x00\x00"
  end

  it 'should move data into the buffer' do
    contents = @buffer.contents
    contents[0] = 15123123
    @buffer.repack contents
    expect(@buffer.raw_data.byteslice(0,4).unpack('i')).to eq [15123123]
  end

  it 'should convert 32bit ints to floats from 0 to 1' do
    expect(@buffer.to_float(0)).to be_zero
    expect(@buffer.to_float(0x7FFFFFFF).round(9)).to eq 1.0
  end
end
