/*
* Part of ntpl
* ntpl.modificators v.0.1.0
* Copyright 2010, Fedor Indutny
* Released under MIT license
*/
#include <ntpl.mod.h>
#include <v8.h>
#include <stdio.h>


namespace ntpl {
	namespace mod {
		using namespace v8;

		Persistent<FunctionTemplate> constructor_template;
		
		enum OPS_STATE {
			STAND_BY = 0,
			NAME = 1,
			WAITING_ARRAY = 2,
			WAITING_VAL = 3
		};
		
		#define _PARSER_OP(offset, char) (input[i+offset] == char)
		#define _PARSER_MOVE(offset) last=(i+=offset)
		#define _GET_SYMB_PART String::NewSymbol( (char*) input + last, i - last )
		#define _GET_PART String::New( (char*) input + last, i - last )
		#define _PARSER_OP_SPACES(offset) (_PARSER_OP(0,' ') || _PARSER_OP(0,'\t'))
		#define _SKIP_SPACES do{i++;}while(_PARSER_OP_SPACES(0));_PARSER_MOVE(0);		
		// "Set" modificator
		Handle<Value> option(const Arguments& args)
		{
			HandleScope scope;
			
			// Get arguments
			Local<String> str = Local<String>::Cast(args[0]);
			Local<Object> options = Local<Object>::Cast(args[2]);
			
			// Convert string
			String::Utf8Value str_(str);
			unsigned char* input = (unsigned char*) *str_;
			
			// Init state
			OPS_STATE state = STAND_BY;
			
			// Init pos
			int last = 0;
			
			// Here name will be stored
			Local<String> name;
			Local<Object> lastOption = options;

			// Here will be stored value and value of Array
			Local<Value> value;
			Local<Array> valueA;
			
			// Array index
			int valLength = 0;
			
			for (int i = 0; input[i] ; )
				// Found start of option name
				if (state == STAND_BY && !_PARSER_OP_SPACES(0))
				{					
					_PARSER_MOVE(0); // last = current
					state = NAME;
				}
				else if (state == NAME && _PARSER_OP(0, '.'))
				{
					Local<String> name_ = _GET_SYMB_PART;				
					Local<Value> lastOption_ = lastOption->Get(name_);
					if (!lastOption_->IsObject())
					{
						Local<Object> newLastOption = Object::New();
						lastOption->Set(name_, newLastOption);
						lastOption = newLastOption;
					} else 
					{
						lastOption = lastOption_->ToObject();
					}
					_PARSER_MOVE(1);
				}
				// Name ends - store it, check if value must be array or not
				// And wait for value
				else if (state == NAME && _PARSER_OP_SPACES(0))
				{
					// Create symbol
					name = _GET_SYMB_PART;
					// Get value
					value = lastOption->Get(name);
					
					// Is array?
					if (value->IsArray())
					{
						state = WAITING_ARRAY;
						// Convert
						valueA = Local<Array>::Cast(value);
						// Cache array last index
						valLength = valueA->Length();
						
					} else
					{
						state = WAITING_VAL;
					}
					
					// Skip leading spaces
					_SKIP_SPACES;
				}
				// If we're waiting for array's values and
				// getting " ","\t","," or "EOF" character
				// Push value to array
				else if (
					(
						state == WAITING_ARRAY &&
						(_PARSER_OP_SPACES(0) ||
						_PARSER_OP(0, ',') || !input[i+1]) 
					)
					||
					(
						state == WAITING_VAL &&
						_PARSER_OP(0, ',')
					)
				) 
				{
					// If we was waiting for value - redefine state
					if (state == WAITING_VAL)
					{
						state = WAITING_ARRAY;
						// Initiate array
						valLength = 0;
						valueA = Array::New();
						
						// If not empty
						if (!value->IsUndefined())
						{
							// Get old value and push it
							Local<String> oldValue = value->ToString();
							// And increment index
							valueA->Set(valLength++, oldValue);
						}
						
						// Add it to option
						lastOption->Set(name, valueA);
					}
					
					// If next symbol is "EOF" or ";"
					// And now we have not " " and not "," symbol
					// Move to next - to copy current symbol too
					if (!_PARSER_OP_SPACES(0) && !_PARSER_OP(0, ',') 
						&& !input[i+1]  )
					{
						i++;
					}
					
					// Get buffered input
					Local<String> t = _GET_PART;
					
					// Value can't be ""
					if (t->Length())
					{
						valueA->Set(valLength++, t);
					}
					
					// If we have "EOF" - break
					if (!input[i])
					{
						break;
					}
					
					// Skip spaces
					_SKIP_SPACES;					
				}
				// Simply move forward and set value
				// If get "EOF" next
				else if (state == WAITING_VAL && !input[i+1] )
				{
					// Remove spaces and tabs from the end of text
					while ( _PARSER_OP_SPACES(0) && i > last)
					{
						i--;
					}
					// Last symbol won't be copied so go slightly forward
					i++;
					
					Local<String> valueS = _GET_PART;
					
					// If value isn't empty
					if (valueS->Length())
					{
						lastOption->Set(name, _GET_PART);
					}
					break;
				}				
				// Bufferize it
				else
				{
					i++;
				}
			
			// Return empty string
			return scope.Close(String::New(""));
		}
		#undef _PARSER_OP
		#undef _PARSER_MOVE
		#undef _GET_SYMB_PART
		#undef _GET_PART
		#undef _PARSER_OP_SPACES
		#undef _SKIP_SPACES
		
		#define NEW_MODIFICATOR(name,func) modificators->Set( String::New(name), FunctionTemplate::New(func)->GetFunction() )
		
		// This function adds modificator to current ntpl instance
		Handle<Value> add(const Arguments& args)
		{
			HandleScope scope;
			
			// First object must be modificators object
			if (!args[0]->IsObject())
			{
				return scope.Close(Object::New());
			}
			
			// Cast like object
			Local<Object> modificators = args[0]->ToObject();
			
			// Set modificator
			NEW_MODIFICATOR("set", option);
			
			// Return modificators
			return scope.Close(modificators);
		}
		
		// This function adds "initModificators" to module context
		void init(Handle<Object> target)
		{
			HandleScope scope;
			
			Local<FunctionTemplate> t = FunctionTemplate::New(add);
			constructor_template = Persistent<FunctionTemplate>::New(t);			
			
			target->Set(String::NewSymbol("initModificators"), constructor_template->GetFunction());
		}
		#undef NEW_MODIFICATOR
	}
}
