import os
import sys

import fileinput

path = os.path.dirname(os.path.abspath(__file__)) + "\\"
subdirh = "EvayrNet\\include\\data\\messages\\"
subdircpp = "EvayrNet\\src\\data\\messages\\"
log = open(path + "codegen log.txt",'w')

space = " "
semicolon = ";"
newline = "\n"
tab = "\t"

packetSize = 0
stringLines = []
stringPreLines = []
headerSize = 6
messageOpcode = 0

serializationLines = []
deserializationLines = []

def Log(input):
	print(input + newline)
	log.write(input + newline)
	return

def tabs(amount):
	i = 0
	output = ""
	while i < amount:
		output = output + tab
		i += 1
	return output
	
def GetLastLineNumber(file):
	global cpp
	cpp.seek(0)
	length = len(cpp.readlines())
	Log("Length: " + str(length))
	return length
	
def StartHeaderFile():
	global h
	h.truncate()
	h.write("/* This file was automatically generated. Do not modify! */\n")
	h.write("/* For more info, find protocol.txt / codegen.py or contact @Ivar Slotboom */\n\n")
	
	h.write("#ifndef _MESSAGES_H_\n")
	h.write("#define _MESSAGES_H_\n")
	h.write(newline)
	h.write("#include <vector>\n")
	h.write("#include <string>\n")
	h.write(newline)
	h.write("#include \"data\\messages\\MessageTypes.h\"\n")
	h.write(newline)
	h.write("#include \"data\\stream\\DataStreamReader.h\"\n")
	h.write("#include \"data\\stream\\DataStreamWriter.h\"\n")
	h.write(newline)
	h.write("namespace EvayrNet\n")
	h.write("{\n")
	h.write("namespace Messages\n")
	h.write("{\n")
	h.write(newline)
	h.write("class Message\n")
	h.write("{\n")
	h.write("public:\n")
	h.write(tab + "Message::Message();\n")
	h.write(tab + "Message::~Message();\n")
	h.write(newline)
	h.write(tab + "virtual void Serialize(EvayrNet::DataStreamWriter& aWriter){}\n")
	h.write(tab + "virtual void Deserialize(EvayrNet::DataStreamReader& aReader){}\n")
	h.write(tab + "virtual void Execute(){}\n")
	h.write(newline)
	h.write(tab + "virtual uint16_t GetMessageSize(){ return 0; }\n")
	h.write(tab + "virtual uint8_t GetMessageOpcode(){ return 0; }\n")
	h.write(tab + "virtual const char* GetMessageName() { return \"Message\"; }\n")
	h.write(newline)
	h.write(tab + "Messages::EMessageType m_MessageType;\n")
	h.write(tab + "uint8_t m_SequenceID; // If this is 0, there is no sequence\n")
	h.write(tab + "uint16_t m_ConnectionID; // ConnectionID from the sender\n")
	h.write("};\n")
	h.write(newline)
	return
	
def EndHeaderFile():
	global h
	h.write("} // namespace Messages\n")
	h.write("} // namespace EvayrNet\n")
	h.write(newline)
	h.write("#endif")
	h.write(newline)
	return
	
def StartSourceFile():
	global cpp
	cpp.truncate()
	
	cpp.write("/* This file was automatically generated. Do not modify! */\n")
	cpp.write("/* For more info, find protocol.txt / codegen.py or contact @Ivar Slotboom */\n\n")
	
	cpp.write("#include \"data\\messages\\Messages.h\"\n")
	cpp.write(newline)
	cpp.write("namespace EvayrNet\n")
	cpp.write("{\n")
	cpp.write("namespace Messages\n")
	cpp.write("{\n")
	cpp.write(newline)
	cpp.write("Message::Message() : m_SequenceID(0), m_ConnectionID(0) {}\n")
	cpp.write("Message::~Message() {}\n")
	cpp.write(newline)
	return
	
def EndSourceFile():
	global cpp
	cpp.write("} // namespace Messages\n")
	cpp.write("} // namespace EvayrNet\n")
	cpp.write(newline)
	return

def StartMessage(messageName, opcode):
	global packetSize
	packetSize = 0
	
	global h
	h.write("class " + messageName + " : public Message" + newline)
	h.write("{" + newline)
	h.write("public:" + newline)
	h.write(tab + messageName + "();" + newline)
	h.write(tab + "~" + messageName + "();\n")
	h.write(newline)	
	h.write(tab + "void Serialize(EvayrNet::DataStreamWriter& aWriter);\n")
	h.write(tab + "void Deserialize(EvayrNet::DataStreamReader& aReader);\n")
	h.write(tab + "void Execute();\n")
	h.write(newline)
	h.write(tab + "uint16_t GetMessageSize();\n")
	h.write(tab + "uint8_t GetMessageOpcode();\n")
	h.write(tab + "const char* GetMessageName();\n")
	h.write(newline)
	
	global cpp
	cpp.write(messageName + "::" + messageName + "()" + newline)
	cpp.write("{" + newline)
	cpp.write("}" + newline)
	cpp.write(newline)
	
	cpp.write(messageName + "::~" + messageName + "()" + newline)
	cpp.write("{" + newline)
	cpp.write("}" + newline)
	cpp.write(newline)
	
	Log("New message name=\"" + messageName + "\"" + newline)
	return
	
def EndMessage(messageName, opcode):
	global cpp
	
	cpp.write("void " + messageName + "::Serialize(EvayrNet::DataStreamWriter& aWriter)\n")
	cpp.write("{\n")
	cpp.write(tab + "// Serialize header\n")
	cpp.write(tab + "aWriter.Write(GetMessageSize()); // message size\n")
	cpp.write(tab + "aWriter.Write(uint8_t(" + str(opcode) + ")); // opcode\n")
	cpp.write(tab + "aWriter.Write(m_SequenceID); // sequence ID\n")
	cpp.write(tab + "aWriter.Write(m_ConnectionID); // connection ID\n")
	cpp.write(newline)
	cpp.write(tab + "// Serialize member variables\n")
	si = 0;
	while si < len(serializationLines):
		cpp.write(serializationLines[si])
		si += 1
	cpp.write("}\n")
	cpp.write(newline)
	
	cpp.write("void " + messageName + "::Deserialize(EvayrNet::DataStreamReader& aReader)\n")
	cpp.write("{\n")
	cpp.write(tab + "// Deserialize member variables\n")
	di = 0;
	while di < len(deserializationLines):
		cpp.write(deserializationLines[di])
		di += 1
	cpp.write("}\n")
	cpp.write(newline)
	cpp.write("void " + messageName + "::Execute()\n")
	cpp.write("{\n")
	cpp.write(tab + messageName + "_Receive(*this);\n")
	cpp.write("}\n")
	cpp.write(newline)
	global packetSize
	cpp.write("uint16_t " + messageName + "::GetMessageSize()\n")
	cpp.write("{\n")
	if len(stringPreLines) > 0:
		cpp.write(tab + "uint16_t arraysSize = 0;\n")
		strpi = 0;
		while strpi < len(stringPreLines):
			cpp.write(stringPreLines[strpi])
			strpi += 1
		cpp.write(newline)
	cpp.write(tab + "return " + str(packetSize + headerSize))
	stri = 0;
	while stri < len(stringLines):
		cpp.write(stringLines[stri])
		stri += 1
	if len(stringPreLines) == 0:
		cpp.write(";\n")
	else:
		cpp.write(" + arraysSize;\n")
	cpp.write("}\n")
	cpp.write(newline)
	cpp.write("uint8_t " + messageName + "::GetMessageOpcode()\n")
	cpp.write("{\n")
	cpp.write(tab + "return " + str(opcode) + ";\n")
	cpp.write("}\n")
	cpp.write(newline)
	cpp.write("const char* " + messageName + "::GetMessageName()\n")
	cpp.write("{\n")
	cpp.write(tab + "return \"" + messageName + "\";\n")
	cpp.write("}\n")
	cpp.write(newline)
	Log("End message" + newline)

	global h
	h.write('};\n')
	h.write("extern void " + messageName + "_Receive(const Messages::" + messageName + "& acMessage);\n")
	h.write(newline)
	return
	
def GetCPPType(type):
	if type == "int8" or type == "uint8" or type == "int16" or type == "uint16" or type == "int32" or type == "uint32" or type == "int64" or type == "uint64":
		return type + "_t"
	elif type == "string":
		return "std::string"
	else:
		return type
	
def WriteMember(type, name, isArray):
	Log("New member type=\"" + GetCPPType(type) + "\" name=\"" + name + "\" array=\"" + isArray + "\"" + newline)

	global h
	if (isArray == "no"):
		h.write(tab + GetCPPType(type) + space + name + semicolon + newline)
	else:
		h.write(tab + "std::vector<" + GetCPPType(type) + ">" + space + name + semicolon + newline)
	
	global packetSize
	if type == "int8" or type == "uint8" or type == "char" or type == "bool":
		if isArray == "no":
			packetSize += 1
		else:
			stringPreLines.append(tab + "arraysSize += uint16_t(" + name + ".size()) * " + str(1) + semicolon + newline)
	elif type == "int16" or type == "uint16" or type == "short":
		if isArray == "no":
			packetSize += 2
		else:
			stringPreLines.append(tab + "arraysSize += uint16_t(" + name + ".size()) * " + str(2) + semicolon + newline)
	elif type == "float" or type == "uint32" or type == "int32" or type == "long":
		if isArray == "no":
			packetSize += 4
		else:
			stringPreLines.append(tab + "arraysSize += uint16_t(" + name + ".size()) * " + str(4) + semicolon + newline)
	elif type == "uint64" or type == "int64" or type == "double":
		if isArray == "no":
			packetSize += 8
		else:
			stringPreLines.append(tab + "arraysSize += uint16_t(" + name + ".size()) * " + str(8) + semicolon + newline)
	elif type == "string":
		if isArray == "no":
			stringLines.append(" + " + name + ".size() + 2")
		else:
			stringPreLines.append(tab + "for (size_t i = 0; i < " + name + ".size(); ++i) {arraysSize += uint16_t(" + name + "[i].size() + 2); }\n")
	return

Log("Started")
log.truncate()
output = ""
origin = ""
filename = "Protocol.txt"
Log("Opening f file")
f = open(path + filename, 'r')
Log("Opening cpp file")
cpp = open(path + subdircpp + 'messages.cpp','w+')
Log("Opening h file")
h = open(path + subdirh + 'messages.h','w+')

Log("Truncating")

cpp.truncate()
h.truncate()

Log("After open")
if f:
	Log("Filepath: \"" + os.path.dirname(os.path.abspath(__file__)) + "\"")
	
	StartHeaderFile()
	StartSourceFile()
	Log("Start of generation path=\"" + path + "\"\n")
	message=""
	
	for line in f:
		Log("Line")
		
		wordcount = 0
		type = ''
		name = ''
		
		for word in line.split():
			Log("Processing word: \"" + word + "\"")
			if message == "":
				message = word
				global serializationLines
				global deserializationLines
				serializationLines = []
				deserializationLines = []
				stringLines = []
				stringPreLines = []
				StartMessage(message, messageOpcode)
			
			elif word == "end":
				EndMessage(message, messageOpcode)
				message = ""
				messageOpcode += 1
		
			else:
				if wordcount == 0:
					type = word
				elif wordcount == 1:
					name = word
					if len(line.split()) == 2:
						serializationLines.append(tab + "aWriter.Write(" + name + ");\n")
						deserializationLines.append(tab + "aReader.Read(" + name + ");\n")
						WriteMember(type, name, "no")
						
					else:
						serializationLines.append(tab + "aWriter.Write(uint16_t(" + name + ".size()));\n")
						serializationLines.append(tab + "for (size_t i = 0; i < " + name + ".size(); ++i)\n")
						serializationLines.append(tab + "{\n")
						serializationLines.append(tabs(2) + "aWriter.Write(" + name + "[i]);\n")
						serializationLines.append(tab + "}\n\n")
						
						deserializationLines.append(tab + "uint16_t " + name + "_count = 0;\n")
						deserializationLines.append(tab + "aReader.Read(" + name + "_count);\n")
						deserializationLines.append(tab + "for (size_t i = 0; i < " + name + "_count; ++i)\n")
						deserializationLines.append(tab + "{\n")
						deserializationLines.append(tabs(2) + "" + GetCPPType(type) + " t;\n")
						deserializationLines.append(tabs(2) + "aReader.Read(t);\n")
						deserializationLines.append(tabs(2) + "" + name + ".push_back(t);\n")
						deserializationLines.append(tab + "}\n")
						WriteMember(type, name, "yes")
				wordcount += 1
				
	EndHeaderFile()
	EndSourceFile()
	Log("Finished generating code :)")
else:
	Log("Failed to open \"" + filepath + "\"")