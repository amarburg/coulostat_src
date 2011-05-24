#!/usr/bin/ruby


skip_next = false
on_font = false
char_array = []
num_rows = 0
num_cols = 0
font_name = ""

ARGF.each_line { |line|


  if on_font
    if line.include?("};")
      on_font = false 
      puts line

      bytes_per = ((num_cols/8.0).ceil * num_rows).to_i
      puts <<EODEF

const FONT_CHAR_INFO #{font_name}_Descriptors[] =
{
EODEF
char_array.each_index { |c| puts "  {#{num_cols},#{num_rows},#{c*bytes_per}},      // #{char_array[c]}" }
puts <<EODEF2
};

const FONT_INFO #{font_name}_Info =
{
  #{ (num_rows/8.0).ceil }, /*  Character height in bytes */
  '#{char_array.first.strip}', /*  Start character */
  '#{char_array.last.strip}', /*  End character */
  #{num_cols}, /*  Width, in pixels, of space character */
  #{font_name}_Descriptors, /*  Character decriptor array */
  #{font_name}, /*  Character bitmap array */
};
EODEF2

    else
      if !skip_next 
        bytes,char = line.split("//")

        bytes = bytes.split(",")

        while bytes.length % 8 != 0 do bytes.pop end
        bytes.collect! { |s| s.strip }

        if num_cols <= 8
          bytes.reverse!
        else

          puts "byte = #{bytes.join(',')}"

          unflat_array = Array.new( num_rows ) { |i|
            Array.new( (num_cols/8.0).ceil.to_i ) { |j|
              bytes.unshift } }

          puts "unfl = #{unflat_array.join(',')}"
              bytes = unflat_array.reverse.flatten
        end

        puts "#{bytes.join(',')}, // #{char}"

        char_array << char

      else
        bytes = line.split(',')
        num_cols = bytes[0].hex
        num_rows = bytes[1].hex

        skip_next = false
      end
    end

  else


    if line.include?("FONT")
      skip_next = true
      on_font = true
      char_array = []
      font_name = line[/FONT\w+/]

    end

    puts line
  end
}

