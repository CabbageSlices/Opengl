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
            entity->registerUpdateCallback(mockComponent, 
                [&](){updateCallback.Call();}
            );
            entity->registerRenderCallback(mockComponent, 
                [&](){renderCallback.Call();}
            );
        }
    );

    EXPECT_CALL(*mockComponent2, registerCallbacksToEntity(&entity)).WillOnce(
        [&](Entity *entity){
            entity->registerUpdateCallback(mockComponent2, 
                [&](){updateCallback.Call();}
            );
            entity->registerRenderCallback(mockComponent2, 
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
            entity->registerUpdateCallback(mockComponent, 
                [&](){updateCallback.Call();}
            );
            entity->registerRenderCallback(mockComponent, 
                [&](){renderCallback.Call();}
            );
        }
    );

    EXPECT_CALL(*mockComponent2, registerCallbacksToEntity(&entity)).WillOnce(
        [&](Entity *entity){
            entity->registerUpdateCallback(mockComponent2, 
                [&](){updateCallback.Call();}
            );
            entity->registerRenderCallback(mockComponent2, 
                [&](){renderCallback.Call();}
            );
        }
    );
    
    EXPECT_CALL(updateCallback, Call()).Times(1);
    EXPECT_CALL(renderCallback, Call()).Times(1);

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
            entity->registerUpdateCallback(mockComponent, 
                [&](){updateCallback.Call();}
            );
            entity->registerRenderCallback(mockComponent, 
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
            entity->registerUpdateCallback(mockComponent, 
                [&](){updateCallback.Call();}
            );
            entity->registerRenderCallback(mockComponent, 
                [&](){renderCallback.Call();}
            );
            entity->registerUpdateCallback(mockComponent, 
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