#ifndef __CORE_INPUT_INPUT_CONTROLLER_H_
#define __CORE_INPUT_INPUT_CONTROLLER_H_

#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

#include "inputAction.h"

namespace core
{

class InputActionBinding
{
public:
	InputActionBinding(const InputAction& action, InputTrigger triggerState) : mAction(action), mTrigger(triggerState)
	{
	}
	virtual ~InputActionBinding() = default;
	virtual bool execute(const InputValue& inputValue)
	{
		return false;
	}

	InputTrigger getInputTrigger() const
	{
		return mTrigger;
	}
	const InputAction& getAction() const
	{
		return mAction;
	}

	bool operator<(const InputActionBinding& other) const
	{
		return mAction.mPriority < other.mAction.mPriority;
	}

	bool mIsDeleted{false};

private:
	const InputAction mAction;
	InputTrigger mTrigger{};
};

template <typename Func>
class InputActionFunctionBinding : public InputActionBinding
{
public:
	InputActionFunctionBinding(Func func, const InputAction& action, InputTrigger triggerState)
		: InputActionBinding(action, triggerState), mFunc(func)
	{
	}
	virtual ~InputActionFunctionBinding() = default;

	virtual bool execute(const InputValue& inputValue) override
	{
		return mFunc(inputValue);
	}

private:
	Func mFunc;
};

template <typename TClass, typename Func>
class InputActionMemberFunctionBinding : public InputActionBinding
{
public:
	InputActionMemberFunctionBinding(TClass* classPointer,
									 Func func,
									 const InputAction& action,
									 InputTrigger triggerState)
		: InputActionBinding(action, triggerState), rClassPointer(classPointer), mFunc(func)
	{
	}
	virtual ~InputActionMemberFunctionBinding() = default;

	virtual bool execute(const InputValue& inputValue) override
	{
		return (rClassPointer->*mFunc)(inputValue);
	}

private:
	TClass* rClassPointer;
	Func mFunc;
};

class InputController
{
public:
	template <typename TClass>
	InputActionBinding* bindAction(const InputAction& action,
								   InputTrigger triggerState,
								   TClass* object,
								   bool (TClass::*func)(const InputValue& inputValue))
	{
		auto* actionEvent =
			new InputActionMemberFunctionBinding<TClass, decltype(func)>(object, func, action, triggerState);
		mBindingActions.push_back(actionEvent);
		return actionEvent;
	}
	InputActionBinding* bindAction(const InputAction& action,
								   InputTrigger triggerState,
								   bool (*func)(const InputValue& inputValue))
	{
		auto* actionEvent = new InputActionFunctionBinding<decltype(func)>(func, action, triggerState);
		mBindingActions.push_back(actionEvent);
		return actionEvent;
	}
	void unbinding(InputActionBinding* actionBinding)
	{
		actionBinding->mIsDeleted = true;
		mUnbindingActions.push_back(actionBinding);
	}
	void sync()
	{
		bool isChanged = mBindingActions.size() > 0 || mUnbindingActions.size() > 0;
		std::unordered_set<InputType> inputTypes;

		for (auto* actionBinding : mBindingActions)
		{
			inputTypes.insert(actionBinding->getAction().getType());
			mInputActions[actionBinding->getAction().getType()].emplace_back(actionBinding);
		}
		mBindingActions.clear();

		for (auto* actionBinding : mUnbindingActions)
		{
			inputTypes.insert(actionBinding->getAction().getType());
			auto& actions = mInputActions[actionBinding->getAction().getType()];
			auto it = std::remove_if(actions.begin(), actions.end(),
									 [actionBinding](InputActionBinding* binding) { return binding == actionBinding; });
			assert(it != actions.end());

			delete actionBinding;
			actions.erase(it, actions.end());
		}
		mUnbindingActions.clear();

		if (isChanged)
		{
			for (auto& inputType : inputTypes)
			{
				auto& inputActionBindings = mInputActions[inputType];
				std::sort(inputActionBindings.begin(), inputActionBindings.end(),
						  [](const InputActionBinding* a, const InputActionBinding* b) { return *a < *b; });
			}
		}
	}
	void broadcast(InputType inputType, InputTrigger triggerState, const InputValue& value)
	{
		bool isCaptured = false;
		auto& inputActionBindings = mInputActions[inputType];
		if (mBeforeInputValue.find(inputType) == mBeforeInputValue.end())
		{
			mBeforeInputValue[inputType] = value.get<Vec3>();
		}

		auto tempValue = value;
		tempValue.mBeforeValue = mBeforeInputValue[inputType];

		for (auto it = inputActionBindings.rbegin(); it != inputActionBindings.rend(); ++it)
		{
			auto* inputActionBinding = *it;
			if (HasFlag(inputActionBinding->getInputTrigger(), triggerState))
			{
				if (inputActionBinding->mIsDeleted || (inputActionBinding->getAction().mUseCapture && isCaptured))
					continue;
				isCaptured |= inputActionBinding->execute(tempValue);
			}
		}
		mBeforeInputValue[inputType] = value.get<Vec3>();
	}
	size_t size() const
	{
		return mInputActions.size();
	}
	void clearCach()
	{
		mBeforeInputValue.clear();
	}

private:
	std::unordered_map<InputType, Vec3> mBeforeInputValue;
	std::unordered_map<InputType, std::vector<InputActionBinding*>> mInputActions;
	std::vector<InputActionBinding*> mUnbindingActions;
	std::vector<InputActionBinding*> mBindingActions;
};

}	 // namespace core

#endif