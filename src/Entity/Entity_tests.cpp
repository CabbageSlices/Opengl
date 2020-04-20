#include "./Entity_tests.h"
#include "gmock/gmock-function-mocker.h"

using ::testing::MockFunction;
using testing::Mock;

//test that adding component to entity results in the entity having the component
//and the register callback method is called
//and then test the update/render callbacks
TEST_F(EntityTest, addComponentToEntity) {

    EXPECT_CALL(*mockComponent, registerCallbacksToEntity(&entity)).WillOnce(
        [&](Entity *entity){
            entity->registerUpdateCallback(mockComponent.get(), 
                [&](){updateCallback.Call();}
            );
            entity->registerRenderCallback(mockComponent.get(), 
                [&](){renderCallback.Call();}
            );
        }
    );

    EXPECT_CALL(*mockComponent2, registerCallbacksToEntity(&entity)).WillOnce(
        [&](Entity *entity){
            entity->registerUpdateCallback(mockComponent2.get(), 
                [&](){updateCallback.Call();}
            );
            entity->registerRenderCallback(mockComponent2.get(), 
                [&](){renderCallback.Call();}
            );
        }
    );
    
    EXPECT_CALL(updateCallback, Call()).Times(2);
    EXPECT_CALL(renderCallback, Call()).Times(2);

    ASSERT_TRUE(entity.addComponent(mockComponent));
    ASSERT_TRUE(entity.addComponent(mockComponent2));
    entity.render();
    entity.update();
}

//test removing component from entity, should remove all callbacks
TEST_F(EntityTest, removeComponent) {

    EXPECT_CALL(*mockComponent, registerCallbacksToEntity(&entity)).WillOnce(
        [&](Entity *entity){
            entity->registerUpdateCallback(mockComponent.get(), 
                [&](){updateCallback.Call();}
            );
            entity->registerRenderCallback(mockComponent.get(), 
                [&](){renderCallback.Call();}
            );
        }
    );

    EXPECT_CALL(*mockComponent2, registerCallbacksToEntity(&entity)).WillOnce(
        [&](Entity *entity){
            entity->registerUpdateCallback(mockComponent2.get(), 
                [&](){updateCallback.Call();}
            );
            entity->registerRenderCallback(mockComponent2.get(), 
                [&](){renderCallback.Call();}
            );
        }
    );

    EXPECT_CALL(updateCallback, Call()).Times(0);
    EXPECT_CALL(renderCallback, Call()).Times(0);

    ASSERT_TRUE(entity.addComponent(mockComponent));
    ASSERT_TRUE(entity.addComponent(mockComponent2));
    
    entity.removeComponent(mockComponent);
    entity.removeComponent(mockComponent2.get());

    entity.render();
    entity.update();
}

TEST_F(EntityTest, removeAllComponents) {

    EXPECT_CALL(*mockComponent, registerCallbacksToEntity(&entity)).WillOnce(
        [&](Entity *entity){
            entity->registerUpdateCallback(mockComponent.get(), 
                [&](){updateCallback.Call();}
            );
            entity->registerRenderCallback(mockComponent.get(), 
                [&](){renderCallback.Call();}
            );
        }
    );

    EXPECT_CALL(*mockComponent2, registerCallbacksToEntity(&entity)).WillOnce(
        [&](Entity *entity){
            entity->registerUpdateCallback(mockComponent2.get(), 
                [&](){updateCallback.Call();}
            );
            entity->registerRenderCallback(mockComponent2.get(), 
                [&](){renderCallback.Call();}
            );
        }
    );

    EXPECT_CALL(updateCallback, Call()).Times(0);
    EXPECT_CALL(renderCallback, Call()).Times(0);

    ASSERT_TRUE(entity.addComponent(mockComponent));
    ASSERT_TRUE(entity.addComponent(mockComponent2));
    
    entity.removeAllComponents();

    entity.render();
    entity.update();
}

//test that component remains in entity even if it's out of scope
TEST_F(EntityTest, componentRemainOutOfScop) {

    EXPECT_CALL(updateCallback, Call()).Times(2);
    EXPECT_CALL(renderCallback, Call()).Times(2);

    //test temp component that get's deleted
    {
        std::shared_ptr<MockComponent> tempComponent(new MockComponent());

        EXPECT_CALL(*tempComponent, registerCallbacksToEntity(&entity)).WillOnce(
            [&](Entity *entity){
                entity->registerUpdateCallback(tempComponent.get(), 
                    [&](){updateCallback.Call();}
                );
                entity->registerRenderCallback(tempComponent.get(), 
                    [&](){renderCallback.Call();}
                );
            }
        );
        ASSERT_TRUE(entity.addComponent(tempComponent));
    }
    //entity should contain component since it stores shared ptr
    ASSERT_EQ(entity.components.size(), 1);
    ASSERT_EQ(entity.renderCallbacks.size(), 1);
    ASSERT_EQ(entity.updateCallbacks.size(), 1);

    EXPECT_CALL(*mockComponent, registerCallbacksToEntity(&entity)).WillOnce(
        [&](Entity *entity){
            entity->registerUpdateCallback(mockComponent.get(), 
                [&](){updateCallback.Call();}
            );
            entity->registerRenderCallback(mockComponent.get(), 
                [&](){renderCallback.Call();}
            );
        }
    );

    EXPECT_CALL(*mockComponent2, registerCallbacksToEntity(&entity)).WillOnce(
        [&](Entity *entity){
            entity->registerUpdateCallback(mockComponent2.get(), 
                [&](){updateCallback.Call();}
            );
            entity->registerRenderCallback(mockComponent2.get(), 
                [&](){renderCallback.Call();}
            );
        }
    );

    ASSERT_TRUE(entity.addComponent(mockComponent));
    ASSERT_TRUE(entity.addComponent(mockComponent2));
    
    entity.removeComponent(mockComponent);

    entity.render();
    entity.update();
}

//test adding duplicated components
//component can be added twice but functions should only be registered once
TEST_F(EntityTest, preventDuplicateComponents) {
    EXPECT_CALL(*mockComponent, registerCallbacksToEntity(&entity)).WillOnce(
        [&](Entity *entity){
            entity->registerUpdateCallback(mockComponent.get(), 
                [&](){updateCallback.Call();}
            );
            entity->registerRenderCallback(mockComponent.get(), 
                [&](){renderCallback.Call();}
            );
        }
    );
    
    EXPECT_CALL(updateCallback, Call()).Times(1);
    EXPECT_CALL(renderCallback, Call()).Times(1);

    ASSERT_TRUE(entity.addComponent(mockComponent));
    ASSERT_FALSE(entity.addComponent(copyOfMockComponent));

    entity.render();
    entity.update();

    entity.removeComponent(copyOfMockComponent);

    entity.render();
    entity.update();
}

//test registering multiple update/render callbacks from the same component
//only the latest callback should trigger
TEST_F(EntityTest, duplicateCallbacks) {
    EXPECT_CALL(*mockComponent, registerCallbacksToEntity(&entity)).WillOnce(
        [&](Entity *entity){
            entity->registerUpdateCallback(mockComponent.get(), 
                [&](){updateCallback.Call();}
            );
            entity->registerRenderCallback(mockComponent.get(), 
                [&](){renderCallback.Call();}
            );
            entity->registerUpdateCallback(mockComponent.get(), 
                [&](){updateCallback2.Call();}
            );
        }
    );
    
    EXPECT_CALL(updateCallback, Call()).Times(0);
    EXPECT_CALL(renderCallback, Call()).Times(1);
    EXPECT_CALL(updateCallback2, Call()).Times(1);

    ASSERT_TRUE(entity.addComponent(mockComponent));

    entity.render();
    entity.update();
}