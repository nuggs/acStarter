##
# I've never used ruby before in my life but I had it installed so gave it
# a go with it to see if I could generate a skins list a little easier than
# what I was doing with C...
# If you know Ruby and want to touch this up, feel free to do so
# It sometimes adds extra }, in the json and you have to go through and remove the
# last comma in each array and for the last entry in the file.
#
# To use this file, place it in your assetto corsa content/car directory and run
# ruby acGenSkins.rb from your command line(Need a Ruby aware cmd prompt)
# It should spit some text out to your screen and generate the skins.json file
#
# Make sure you go through the file and remove the extra commas at the end of the
# arrays and for the last object in the json file.  Validate the json at
# http://jsonlint.com to make sure it cleaned it up properly
##

json = open("skins.json", "a")

json.puts "["
Dir.glob('*').each do|f|
	if File.directory? f
		json.puts(sprintf("%-4s{\n%-8s\"car\":\"%s\",\n", "", "", f))
		puts f
		json.puts(sprintf("%-8s\"skins\":[\n", ""))
		Dir.chdir("#{f}/skins")
		Dir.glob('*').each do|s|
			if File.directory? s
				json.puts(sprintf("%-12s\"#{s}\",", ""))
				puts s
			end
		end
		Dir.chdir("../../")
		json.puts(sprintf("%-8s]\n", ""))
	end
	json.puts(sprintf("%-4s},\n", ""))
end
json.puts "]"