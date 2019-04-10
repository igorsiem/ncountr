# Rakefile for basic Dev tasks in qStub
#
# Copyright Igor Siemienowicz 2018
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt)

# --- Config ---

$project_name = "ncountr"
$project_version = "0.1"
$project_description = "Free personal accounting and budgeting"

# --- End Config ---

ENV['QPRJ_PROJECT_NAME'] = $project_name
ENV['QPRJ_PROJECT_VERSION'] = $project_version
ENV['QPRJ_PROJECT_DESCRIPTION'] = $project_description

directory "build"

desc "clean all build artefacts"
task :clean do
    FileUtils.rm_rf "build"
end

desc "run conan to install / generate dependencies"
task :conan => "build" do
    Dir.chdir "build"
    sh "conan install ../src"
    Dir.chdir ".."
end

desc "run cmake to produce platform-specific build files"
task :cmake => :conan do
    Dir.chdir "build"

    cmake_cmd = "cmake "
    ###cmake_cmd += "-DCMAKE_BUILD_TYPE=Debug "
    cmake_cmd += "-G \"Visual Studio 15 2017 Win64\" " \
        if Rake::Win32::windows?
    cmake_cmd += "../src "

    sh cmake_cmd

    Dir.chdir ".."
end

desc "build binaries"
task :bin => :cmake do    
    Dir.chdir "build"

    make_cmd = "make -j8"

    make_cmd =
            "msbuild /m #{$project_name}.sln " +
            "/p:Configuration=Release " +
            "/p:Platform=\"x64\" " +
            "" if Rake::Win32::windows?

    sh make_cmd

    Dir.chdir ".."
end

# Retrieve the location of a package path from conan
def get_conan_package_location(package)
    cmd_str = "conan info #{package} " +
        "--package-filter \"#{package.to_s}*\" " +
        "--paths --only package_folder " +
        ""

    resp_str = `#{cmd_str}`

    resp_str.lines.each do |line|
        return line.split(": ")[1].strip if line.include?("package_folder")
    end
end

# Retrieve the location of Qt from conan
def get_qt_location
    get_conan_package_location("qt/5.12.2@bincrafters/stable")
end

namespace :path do

    task :boost do
        puts "boost package location: " +
            "#{get_conan_package_location("boost/1.69.0@conan/stable")}"
    end

    task :catch2 do
        puts "catch2 package location: " +
            "#{get_conan_package_location("Catch2/2.7.0@catchorg/stable")}"
    end

    task :fmt do
        puts "fmt package location: " +
            "#{get_conan_package_location("fmt/5.3.0@bincrafters/stable")}"
    end

    task :qt do
        puts "Qt package location: " +
            "#{get_conan_package_location("qt/5.12.2@bincrafters/stable")}"
    end

end

# When in *nix, need to set the Qt plugin path to the conan installation
# of Qt.
if !Rake::Win32::windows?
    ENV['QT_PLUGIN_PATH'] = "#{get_qt_location}/plugins"
    # puts "set QT_PLUGIN_PATH to #{ENV['QT_PLUGIN_PATH']}"
end

directory "test-output"

namespace :clean do
    task :test => "test-output" do
        FileUtils.rm_rf Dir.glob("test-output/*")
    end
end

desc "run test suite"
task :test => [:bin, "clean:test"] do
    sh "build/bin/test-#{$project_name}"
end

desc "run the application"
task :run => :bin do

    sh "build/bin/#{$project_name}-gui " +
        "--logging-level DEB " +
        ""

end

namespace :run do

    task :help => :bin do
        sh "build/bin/#{$project_name}-gui --help"
    end
    
end

directory "build/docs"

desc "build doxygen docs"
task :docs => "build/docs" do
    sh "doxygen"
end

desc "build tests, run tests and build docs"
task :all => [:bin, :test, :docs]

task :default => :all
